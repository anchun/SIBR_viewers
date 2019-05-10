#version 420

layout(location = 0) in vec3 in_vertex;
layout(location = 2) in vec3 in_normal;

out vec3 vertex_coord;

void main(void) {
	gl_Position = vec4(in_vertex,1.0);
    vertex_coord  = in_vertex;
}
