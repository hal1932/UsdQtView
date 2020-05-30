#pragma once
#include <vector>
#include <QtOpenGL>
#include "cbuffers.h"
#include "Vertex.h"
#include "gl.h"
#include "NodeRenderer.h"

using namespace PXR_INTERNAL_NS;

class SceneNode {
public:
    SceneNode();
    SceneNode(SceneNode&& other);
    ~SceneNode();
    
    void load(UsdPrim prim, SceneNode* pParent = nullptr);
    void render(Material* pMaterial);

    //const glm::mat4x4& transform() { return cbVertObj_.resource().transform; }

    const UsdPrim& prim() { return prim_; }
    const glm::mat4x4& transform() { return transform_; }

private:
    UsdPrim prim_;
    SceneNode* pParent_;
    std::vector<SceneNode> children_;

    bool isMesh_;
    glm::mat4x4 transform_;
    glm::vec3 color_;

    std::unique_ptr<NodeRenderer> renderer_;
};
