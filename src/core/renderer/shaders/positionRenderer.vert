#version 420

uniform mat4 MVP;

layout(location = 0) in vec3 in_vertex;

out vec3 position;

void main(void) {
	gl_Position = MVP * vec4(in_vertex,1.0);
    position = in_vertex;
}
