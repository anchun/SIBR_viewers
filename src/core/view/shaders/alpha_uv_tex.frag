#version 420

layout(location = 0) out vec4 out_color;
layout(binding = 0) uniform sampler2D input_rgb;

in vec2 out_uv;

uniform float alpha;

void main() {
    out_color = vec4(texture(input_rgb, out_uv).xyz, alpha);
}
