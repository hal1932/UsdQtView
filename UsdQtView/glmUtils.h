#pragma once
#include <pxr/base/gf/vec2f.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec4f.h>
#include <pxr/base/gf/matrix4d.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

using namespace PXR_INTERNAL_NS;

inline glm::vec2 vec2(GfVec2f v) {
    return glm::vec2(v[0], v[1]);
}

inline glm::vec3 vec3(GfVec3f v) {
    return glm::vec3(v[0], v[1], v[2]);
}

inline glm::vec4 vec4(GfVec3f v, float w = 0.f) {
    return glm::vec4(v[0], v[1], v[2], w);
}

inline glm::vec4 vec4(GfVec4f v) {
    return glm::vec4(v[0], v[1], v[2], v[3]);
}

inline glm::mat4x4 mat4x4(GfMatrix4d m) {
    return glm::mat4x4(
        m[0][0], m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3],
        m[3][0], m[3][1], m[3][2], m[3][3]
    );
}