#version 460

layout(std140)
uniform CbVertScene {
    mat4x4 viewProj;
};

layout(std140)
uniform CbVertObj {
    mat4x4 transform;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

out vec2 vTexcoord;

void main(){
  vec4 pos = vec4(position, 1.0);
  gl_Position = viewProj * transform * pos;
  vTexcoord = texcoord;
}
