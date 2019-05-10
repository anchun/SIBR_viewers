#version 420

uniform mat4 proj;

layout(location = 0) in vec3 in_vertex;

out vec3 vertex_coord;

void main(void) {
	gl_Position = proj * vec4(in_vertex,1.0);
    vertex_coord  = in_vertex;
}
