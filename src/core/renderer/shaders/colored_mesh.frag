#version 420

out vec4 out_color;

in vec3 vertColor;

void main(void) {
	out_color = vec4(vertColor, 1.0);
}
