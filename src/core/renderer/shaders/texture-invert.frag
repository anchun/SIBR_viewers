#version 420

layout(binding = 0) uniform sampler2D tex;
layout(location= 0) out vec4 out_color;

in vec2 tex_coord;

void main(void) {
    vec2 texcoord = tex_coord ;
    out_color = vec4(1, 1, 1, 1) - texture(tex,texcoord);
}
