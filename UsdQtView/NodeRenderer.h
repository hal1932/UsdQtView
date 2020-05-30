#pragma once
#include <vector>
#include <QtOpenGL>
#include <boost/noncopyable.hpp>
#include "cbuffers.h"
#include "Vertex.h"
#include "gl.h"

class SceneNode;
class Material;

class NodeRenderer final : public boost::noncopyable {
public:
    NodeRenderer();
    void create(SceneNode* pNode);
    void render(Material* pMaterial);

    bool renderable() { return renderable_; }

private:
    bool renderable_;

    InputLayout<Vertex> inputLayout_;
    BindableBuffer<Vertex> vertices_;
    BindableBuffer<ushort> indices_;

    UniformBlock<CbVertObj> cbVertObj_;
    UniformBlock<CBFlagObj> cbFlagObj_;

    Texture texture_;
};
