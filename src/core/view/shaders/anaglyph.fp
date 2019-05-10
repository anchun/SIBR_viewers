#version 420

layout(binding = 0) uniform sampler2D left;
layout(binding = 1) uniform sampler2D right;
layout(location= 0) out vec4 out_color;

in vec2 vertex_coord;

void main(void) {
    vec2 texcoord = (vertex_coord + vec2(1.0)) / 2.0;
    vec4 cl = texture(left, texcoord);
    vec4 cr = texture(right, texcoord);
    out_color = vec4(cl.r, cr.g, cr.b, 1.0);
}
