#version 420

layout(location = 0) out vec4 out_color;

in vec3 vertex_coord;

void main(void) {
	out_color = vec4(vertex_coord, gl_FragCoord.z);
}
