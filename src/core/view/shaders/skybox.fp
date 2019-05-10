
#version 420

layout(binding = 0) uniform samplerCube in_CubeMap;
layout(location= 0) out vec4 out_Color;

in VSOUT
{
  vec3 tc;
} in_Frag;

void main(void)
{
  out_Color = texture(in_CubeMap, in_Frag.tc);
}
