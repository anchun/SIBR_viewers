#version 450															
layout(location = 0) out vec4 outColor;																		
layout(location = 0) in vec4 position;														
layout(location = 1) in vec3 colors_gs;														
layout(location = 2) in vec2 texCoords_gs;														
layout(location = 3) in vec3 normals_gs;														

void main()															
{			

	if(100.f*position.z<0.001)
		discard;													
	//outColor = vec4(1.f-10.f*position.z, 1.f-10.f*position.z, 1.f-10.f*position.z , 1.0f);	
	outColor = vec4(colors_gs.x, colors_gs.y, colors_gs.z , 1.0f);	
}																		
