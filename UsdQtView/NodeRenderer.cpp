#include "stdafx.h"
#include "NodeRenderer.h"
#include "SceneNode.h"

NodeRenderer::NodeRenderer()
    : inputLayout_(&vertices_),
      vertices_(GL_ARRAY_BUFFER),
      indices_(GL_ELEMENT_ARRAY_BUFFER)
{}

void NodeRenderer::create(SceneNode* pNode) {
    renderable_ = pNode->prim().IsA<UsdGeomMesh>();
    if (!renderable_) {
        return;
    }

    cbVertObj_.create();
    cbVertObj_.resource().transform = pNode->transform();
    cbVertObj_.upload(GL_STATIC_DRAW);

    const UsdGeomMesh mesh(pNode->prim());

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

    const auto mtl = UsdShadeMaterialBindingAPI(pNode->prim()).ComputeBoundMaterial();
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
    inputLayout_.set(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);  // position
    inputLayout_.set(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 12); // uv
}

void NodeRenderer::setMaterial(Material* pMaterial) {
    pMaterial->beginKeywordVariation();
    pMaterial->enableKeyword("ENABLE_DISPLAY_COLOR");
    if (texture_.created()) {
        pMaterial->enableKeyword("ENABLE_TEXTURE_0");
    }
    material_ = pMaterial->endKeywordVariation();
}

void NodeRenderer::render() {
    inputLayout_.bind();

    material_.bindUniformBlock(&cbVertObj_, "CbVertObj");
    material_.bindUniformBlock(&cbFlagObj_, "CbFlagObj");

    if (texture_.created()) {
        material_.bindTexture(&texture_, GL_TEXTURE0, "texture_0");
    }

    indices_.bind();
    gl.glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);
}