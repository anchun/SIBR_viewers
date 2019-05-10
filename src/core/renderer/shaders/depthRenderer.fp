#version 420

out float out_depth;

void main(void) {
	
    out_depth = 2.0*gl_FragCoord.z-1.0;
    //out_color = fragTexCoord.x*vec4(1.0,0.0,0.0,1.0);
}

