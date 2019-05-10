#version 420

layout(location = 0) in vec3 in_vertex;   

void main(void) {
    gl_Position = vec4(in_vertex, 1.0);
}
