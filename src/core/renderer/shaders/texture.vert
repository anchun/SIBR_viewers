#version 420

layout(location = 0) in vec2 in_vertex;
layout(location = 1) in vec2 in_texcoord;

out vec2 tex_coord;

void main(void) {
	gl_Position = vec4(in_vertex, 0.0, 1.0);
	tex_coord = in_texcoord;
}
