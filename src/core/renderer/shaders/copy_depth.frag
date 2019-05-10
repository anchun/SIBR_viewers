#version 420

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform sampler2D image;

in vec4 texcoord;

void main(void)
{
    vec4 color   = texture(image, texcoord.xy);
	out_color = vec4(vec3(color.r), 1.0);
    gl_FragDepth = color.r;
    //gl_FragDepth = color.r == 0? 1.0 : color.r;
}
