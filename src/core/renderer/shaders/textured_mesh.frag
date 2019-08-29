#version 420

uniform sampler2D tex;

out vec4 out_color;

in vec2 vertUV;

void main(void) {
	vec2 uv = vertUV;
	if(uv.x==0.0 && uv.y==0.0){
	out_color = vec4(1.0,1.0,1.0,1.0);
	}
	else{
	uv.y = 1.0 - uv.y; /// \todo TODO: Why Texture are flipped in y ?
	out_color = texture(tex, uv);
	}
}
