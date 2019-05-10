#version 420

layout(location = 0) in vec3 in_vertex;   
layout(location = 3) in vec3 in_normal;  

out vec3 normals;

void main(void) {
    gl_Position = vec4(in_vertex, 1.0);
	normals = in_normal;
}
