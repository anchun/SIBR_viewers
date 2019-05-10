#version 420

uniform mat4 MVP;

layout(location = 0) in vec3 in_vertex;
layout(location = 2) in vec2 in_uv;

out vec2 vertUV;

void main(void) {
	gl_Position = MVP * vec4(in_vertex,1.0);
	
	vertUV = in_uv;
}
