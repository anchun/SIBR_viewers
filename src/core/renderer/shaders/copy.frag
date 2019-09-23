#version 420

layout(location = 0) out vec4 out_color;

uniform sampler2D image;
uniform bool flip = false;

in vec4 texcoord;

void main(void)
{
    vec4 color   = texture(image, flip ? vec2(texcoord.x, 1.0 - texcoord.y) : texcoord.xy);
    out_color    = color;//vec4(color.rgb, 1.0);
    gl_FragDepth = color.w;
}
