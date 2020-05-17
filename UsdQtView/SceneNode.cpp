#include "stdafx.h"
#include "SceneNode.h"
#include <glm/gtc/matrix_transform.hpp>

SceneNode::SceneNode()
    : pParent_(nullptr),
      isMesh_(false),
      inputLayout_(&vertices_),
      vertices_(GL_ARRAY_BUFFER),
      indices_(GL_ELEMENT_ARRAY_BUFFER)
{
    cbVertObj_.resource().transform = glm::mat4x4(1.f);
}

SceneNode::SceneNode(SceneNode&& other)
    : inputLayout_(&vertices_),
      vertices_(GL_ARRAY_BUFFER),
      indices_(GL_ELEMENT_ARRAY_BUFFER)
{
    inputLayout_ = std::move(other.inputLayout_);
    vertices_ = std::move(other.vertices_);
    indices_ = std::move(other.indices_);

    std::swap(prim_, other.prim_);
    std::swap(pParent_, other.pParent_);
    std::swap(children_, other.children_);

    isMesh_ = other.isMesh_;
    std::swap(color_, other.color_);

    cbVertObj_ = std::move(other.cbVertObj_);
    cbFlagObj_ = std::move(other.cbFlagObj_);

    texture_ = std::move(other.texture_);
}

SceneNode::~SceneNode() {
}

void SceneNode::load(UsdPrim prim, SceneNode* pParent) {
    prim_ = prim;
    pParent_ = pParent;

    glm::mat4x4 transform(1.f);
    auto resetStack = false;

    //{
    //    auto variantSets = prim_.GetVariantSets();

    //    for (const auto& [key, value] : variantSets.GetAllVariantSelections()) {
    //        std::cout << key << " " << value << std::endl;
    //    }

    //    std::cout << "==" << prim_.GetName() << std::endl;
    //    for (const auto& name : variantSets.GetNames()) {
    //        auto set = variantSets.GetVariantSet(name);
    //        std::cout << name << " " << set.GetVariantSelection() << std::endl;
    //        for (const auto& varName : set.GetVariantNames()) {
    //            std::cout << "  " << varName << std::endl;
    //        }

    //        const auto selection = set.GetVariantNames()[0];
    //        //variantSets.SetSelection(name, selection);
    //        set.SetVariantSelection(selection);
    //        std::cout << "select: " << set.GetName() << " " << selection << std::endl;
    //    }

    //    for (const auto& [key, value] : variantSets.GetAllVariantSelections()) {
    //        std::cout << key << " " << value << std::endl;
    //    }
    //}

    if (prim.IsA<UsdGeomXformable>()) {
        const auto xformable = UsdGeomXformable(prim);

        GfMatrix4d t;
        xformable.GetLocalTransformation(&t, &resetStack);
        transform = glm::mat4x4(
            t[0][0], t[0][1], t[0][2], t[0][3],
            t[1][0], t[1][1], t[1][2], t[1][3],
            t[2][0], t[2][1], t[2][2], t[2][3],
            t[3][0], t[3][1], t[3][2], t[3][3]
        );
    }

    if (!resetStack && pParent != nullptr) {
        //transform = transform * pParent_->transform();
        transform = pParent_->transform() * transform;
    }

    qDebug() << prim.GetPath().GetString().c_str() << " " << prim.GetTypeName().GetString().c_str() << " " << resetStack;

    cbVertObj_.create();
    cbVertObj_.resource().transform = transform;
    cbVertObj_.upload(GL_STATIC_DRAW);

    for (auto childPrim : prim.GetChildren()) {
        SceneNode child;
        child.load(childPrim, this);
        children_.push_back(std::move(child));
    }

    if (prim.IsA<UsdGeomMesh>()) {
        isMesh_ = true;

        const auto mesh = UsdGeomMesh(prim);
        
        {
            cbFlagObj_.create();
            auto& resource = cbFlagObj_.resource();

            VtArray<GfVec3f> colors;
            mesh.GetDisplayColorAttr().Get(&colors);
            if (colors.size() > 0) {
                const auto r = colors[0][0];
                const auto g = colors[0][1];
                const auto b = colors[0][2];
                resource.displayColor = glm::vec4(r, g, b, 1.f);
            } else {
                resource.displayColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
            }

            cbFlagObj_.upload(GL_STATIC_DRAW);
        }

        const auto mtl = UsdShadeMaterialBindingAPI(prim).ComputeBoundMaterial();
        if (mtl.GetPrim().IsValid()) {
            const auto output = mtl.GetSurfaceOutput();

            UsdShadeConnectableAPI api;
            TfToken name;
            UsdShadeAttributeType type;
            output.GetConnectedSource(&api, &name, &type);

            const auto shader = UsdShadeShader(api.GetPrim());
            const auto diffuseColor = shader.GetInput(TfToken("diffuseColor"));

            diffuseColor.GetConnectedSource(&api, &name, &type);
            const auto texture = UsdShadeShader(api.GetPrim());
            auto file = texture.GetInput(TfToken("file"));

            SdfAssetPath assetPath;
            file.Get(&assetPath);
            
            auto filePath = assetPath.GetResolvedPath();
            if (filePath.length() == 0) {
                filePath = QFileInfo(assetPath.GetAssetPath().c_str()).absoluteFilePath().toStdString();
            }

            texture_.create();
            texture_.load(filePath.c_str());
        }

        VtArray<GfVec3f> points;
        VtArray<int> faceVertCounts, faceVertIndices;
        VtArray<GfVec2f> texcoords;
        mesh.GetPointsAttr().Get(&points);
        mesh.GetFaceVertexCountsAttr().Get(&faceVertCounts);
        mesh.GetFaceVertexIndicesAttr().Get(&faceVertIndices);
        mesh.GetPrimvar(TfToken("st")).Get(&texcoords);

        vertices_.create();
        indices_.create();

        auto& vertices = vertices_.resource();
        auto& indices = indices_.resource();

        bool hasTexcorrds = texcoords.size() == points.size();
        for (auto i = 0; i < points.size(); ++i) {
            if (hasTexcorrds) {
                vertices.emplace_back(points[i], texcoords[i]);
            } else {
                vertices.emplace_back(points[i]);
            }
        }

        size_t i = 0;
        for (auto count : faceVertCounts) {
            if (count == 3) {
                indices.push_back(faceVertIndices[i + 0]);
                indices.push_back(faceVertIndices[i + 1]);
                indices.push_back(faceVertIndices[i + 2]);
                i += 3;
            } else if (count == 4) {
                indices.push_back(faceVertIndices[i + 0]);
                indices.push_back(faceVertIndices[i + 1]);
                indices.push_back(faceVertIndices[i + 2]);
                indices.push_back(faceVertIndices[i + 2]);
                indices.push_back(faceVertIndices[i + 3]);
                indices.push_back(faceVertIndices[i + 0]);
                i += 4;
            }
        }
        vertices_.upload(GL_STATIC_DRAW);
        indices_.upload(GL_STATIC_DRAW);

        inputLayout_.create();
        inputLayout_.set(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        inputLayout_.set(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 12);
    }
}

void SceneNode::render(ShaderPipe* pShaderPipe) {
    if (isMesh_) {
        inputLayout_.bind();

        pShaderPipe->bindUniformBlock(&cbVertObj_, "CbVertObj");
        pShaderPipe->bindUniformBlock(&cbFlagObj_, "CbFlagObj");

        //pShaderPipe->bindTexture(&texture_, GL_TEXTURE0, "texture");

        indices_.bind();
        gl.glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);
    }

    for (auto& child : children_) {
        child.render(pShaderPipe);
    }
}
