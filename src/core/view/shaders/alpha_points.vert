#version 420

layout(location = 0) in vec3 in_vertex;   

uniform mat4 mvp;    
uniform int radius;

void main(void) {
    gl_Position = mvp * vec4(in_vertex, 1.0);
    gl_PointSize = radius;
}
