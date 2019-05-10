#version 420

layout(triangles) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 mvp;    
uniform float normals_size;

void main(void) {
	vec3 a = gl_in[0].gl_Position.xyz;
	vec3 b = gl_in[1].gl_Position.xyz;
	vec3 c = gl_in[2].gl_Position.xyz;

	vec3 tri_normal = normalize(cross(b-a,c-b));
	vec3 tri_center = (a+b+c)/3.0;
	gl_Position = mvp*vec4(tri_center,1.0);
	EmitVertex();
	gl_Position = mvp*vec4(tri_center + normals_size*tri_normal, 1.0);
	EmitVertex();
	EndPrimitive();
}
