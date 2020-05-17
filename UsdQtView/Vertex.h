#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <pxr/base/gf/vec2f.h>
#include <pxr/base/gf/vec3f.h>

using namespace PXR_INTERNAL_NS;

struct Vertex {
    glm::vec3 position;
    glm::vec2 texcoord;

    Vertex(GfVec3f pos) {
        position = glm::vec3(pos[0], pos[1], pos[2]);
    }

    Vertex(GfVec3f pos, GfVec2f uv) {
        position = glm::vec3(pos[0], pos[1], pos[2]);
        texcoord = glm::vec2(uv[0], uv[1]);
    }
};