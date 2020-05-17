#version 460

layout(std140)
uniform CbFlagObj {
    vec4 displayColor;
};

in vec2 vTexcoord;
out vec4 fColor;

uniform sampler2D texture;

void main(){
   fColor = pow(displayColor, vec4(0.454));
   //fColor = displayColor * texture2D(texture, vec2(vTexcoord.x, 1.0 - vTexcoord.y));
}
