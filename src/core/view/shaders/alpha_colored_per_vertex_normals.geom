#version 420

layout(points) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 mvp;    
uniform float normals_size;

in vec3 normals[];                      

void main(void) {
	gl_Position = mvp*(gl_in[0].gl_Position);
	EmitVertex();
	gl_Position = mvp* vec4(gl_in[0].gl_Position.xyz + normals_size*normals[0],1.0);
	EmitVertex();
	EndPrimitive();
}
