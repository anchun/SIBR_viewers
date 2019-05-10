#version 420

layout (location = 0) out vec4 fragColor;

in INTERFACE {
	vec4 col;
	vec2 uv;
} In ;

uniform sampler2D tex;

void main(){
	fragColor = In.col * texture(tex, In.uv);
}
