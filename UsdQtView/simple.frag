//#version 460

layout(std140)
uniform CbFlagObj {
    vec4 displayColor;
} Obj;

in vec2 vTexcoord0;
out vec4 fColor;

uniform sampler2D texture_0;

void main(){
#if defined ENABLE_TEXTURE_0
    fColor = texture2D(texture_0, vec2(vTexcoord0.x, 1.0 - vTexcoord0.y));
#elif defined ENABLE_DISPLAY_COLOR
    fColor = pow(Obj.displayColor, vec4(0.454));
#else
    fColor = vec4(1.0, 0.0, 0.0, 1.0);
#endif
}
