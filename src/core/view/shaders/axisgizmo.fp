#version 420

in vec3 axis_color;
out vec4 out_color;

void main(void) {
    out_color = vec4(axis_color, 1.0);
}
