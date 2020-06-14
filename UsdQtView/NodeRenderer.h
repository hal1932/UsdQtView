#pragma once
#include <boost/noncopyable.hpp>
#include "cbuffers.h"
#include "Vertex.h"
#include "gl.h"
#include "Material.h"

class SceneNode;
class Material;

class NodeRenderer final : public boost::noncopyable {
public:
    NodeRenderer();

    void create(SceneNode* pNode);

    void setMaterial(Material* pMaterial);
    MaterialVariation material() { return material_; }

    bool renderable() { return renderable_; }
    void render();

private:
    MaterialVariation material_;
    bool renderable_ = false;

    InputLayout<Vertex> inputLayout_;
    BindableBuffer<Vertex> vertices_;
    BindableBuffer<ushort> indices_;

    UniformBlock<CbVertObj> cbVertObj_;
    UniformBlock<CBFlagObj> cbFlagObj_;

    std::unique_ptr<Texture> texture_;
};
