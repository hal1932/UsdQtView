#include "stdafx.h"
#include "SceneNode.h"
#include <glm/gtc/matrix_transform.hpp>

SceneNode::SceneNode()
    : pParent_(nullptr),
      isMesh_(false)
{}

SceneNode::SceneNode(SceneNode&& other)
{
    std::swap(prim_, other.prim_);
    std::swap(pParent_, other.pParent_);
    std::swap(children_, other.children_);

    isMesh_ = other.isMesh_;
    std::swap(color_, other.color_);

    renderer_.swap(other.renderer_);
}

SceneNode::~SceneNode() {
}

void SceneNode::load(UsdPrim prim, SceneNode* pParent) {
    prim_ = prim;
    pParent_ = pParent;

    new (&transform_) glm::mat4x4(1.f);
    auto resetStack = false;

    if (prim.IsA<UsdGeomXformable>()) {
        const auto xformable = UsdGeomXformable(prim);

        GfMatrix4d t;
        xformable.GetLocalTransformation(&t, &resetStack);
        transform_ = glm::mat4x4(
            t[0][0], t[0][1], t[0][2], t[0][3],
            t[1][0], t[1][1], t[1][2], t[1][3],
            t[2][0], t[2][1], t[2][2], t[2][3],
            t[3][0], t[3][1], t[3][2], t[3][3]
        );
    }

    if (!resetStack && pParent != nullptr) {
        transform_ = pParent_->transform() * transform_;
    }

    qDebug() << prim.GetPath().GetString().c_str() << " " << prim.GetTypeName().GetString().c_str() << " " << resetStack;

    for (auto childPrim : prim.GetChildren()) {
        SceneNode child;
        child.load(childPrim, this);
        children_.push_back(std::move(child));
    }

    renderer_ = std::make_unique<NodeRenderer>();
    renderer_->create(this);
}

void SceneNode::render(ShaderPipe* pShaderPipe) {
    if (renderer_->renderable()) {
        renderer_->render(pShaderPipe);
    }

    for (auto& child : children_) {
        child.render(pShaderPipe);
    }
}
