//#version 460

layout(std140)
uniform CbVertScene {
    mat4x4 viewProj;
} Scene;

layout(std140)
uniform CbVertObj {
    mat4x4 transform;
} Obj;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord0;

out vec2 vTexcoord0;

void main(){
  vec4 pos = vec4(position, 1.0);
  gl_Position = Scene.viewProj * Obj.transform * pos;
  vTexcoord0 = texcoord0;
}
