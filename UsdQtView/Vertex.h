#pragma once
#include "glmUtils.h"

#pragma pack(push, 1)
struct Vertex {
    glm::vec3 position;
    glm::vec2 texcoord;

    Vertex(GfVec3f pos)
        : position(vec3(pos)), texcoord(glm::vec2(0.0f, 0.0f))
    { }

    Vertex(GfVec3f pos, GfVec2f uv)
        : position(vec3(pos)), texcoord(vec2(uv))
    { }
};
#pragma pack(pop)
