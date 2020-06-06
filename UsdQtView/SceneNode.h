#pragma once
#include <vector>
#include <functional>
#include <QtOpenGL>
#include "cbuffers.h"
#include "Vertex.h"
#include "gl.h"
#include "NodeRenderer.h"

using namespace PXR_INTERNAL_NS;

class RenderQueue;

class SceneNode {
public:
    static void walk(SceneNode* pNode, std::function<void(SceneNode*)> action);

public:
    SceneNode();
    SceneNode(SceneNode&& other);
    ~SceneNode();
    
    void load(UsdPrim prim, SceneNode* pParent = nullptr);
    void setMaterial(Material* pMaterial);
    void traverseRenderable(RenderQueue* pRenderQueue);

    UsdPrim& prim() { return prim_; }
    const UsdPrim& prim() const { return prim_; }

    const glm::mat4x4& transform() { return transform_; }
    const glm::mat4x4& transform() const { return transform_; }

    void reload(bool recursive);

private:
    UsdPrim prim_;
    SceneNode* pParent_;
    std::vector<SceneNode> children_;

    bool isMesh_;
    glm::mat4x4 transform_;
    glm::vec3 color_;

    std::unique_ptr<NodeRenderer> renderer_;
};
