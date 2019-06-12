#version 420

layout(location = 0) in vec3 in_vertex;
layout(location = 2) in vec2 in_uv;

out vec2 out_uv;

uniform mat4 mvp;

void main() {
    out_uv = in_uv;
    gl_Position = mvp * vec4(in_vertex, 1.0);
}
