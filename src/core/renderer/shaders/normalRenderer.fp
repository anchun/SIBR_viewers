#version 420

in vec3 GtoF_normal;
layout(location = 0) out vec4 out_color;

void main(void) {
	
	vec3 colorN=(GtoF_normal+1.0)/2.0;
    out_color = vec4(colorN,1.0);
    //out_color = fragTexCoord.x*vec4(1.0,0.0,0.0,1.0);
}

