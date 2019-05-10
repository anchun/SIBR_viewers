#version 420

uniform mat4 MVP;

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_color;

out vec3 vertColor;

void main(void) {
	gl_Position = MVP * vec4(in_vertex,1.0);
	
	vertColor = in_color;
}
