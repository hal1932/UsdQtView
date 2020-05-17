#pragma once
#include <vector>
#include <QtOpenGL>
#include "cbuffers.h"
#include "Vertex.h"
#include "gl.h"

using namespace PXR_INTERNAL_NS;

class SceneNode {
public:
    SceneNode();
    SceneNode(SceneNode&& other);
    ~SceneNode();
    
    void load(UsdPrim prim, SceneNode* pParent = nullptr);
    void render(ShaderPipe* pShaderPipe);

    const glm::mat4x4& transform() { return cbVertObj_.resource().transform; }

private:
    InputLayout<Vertex> inputLayout_;
    BindableBuffer<Vertex> vertices_;
    BindableBuffer<ushort> indices_;

    UsdPrim prim_;
    SceneNode* pParent_;
    std::vector<SceneNode> children_;

    bool isMesh_;
    glm::vec3 color_;

    UniformBlock<CbVertObj> cbVertObj_;
    UniformBlock<CBFlagObj> cbFlagObj_;

    Texture texture_;
};
