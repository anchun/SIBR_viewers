#version 420

layout(location = 0) out vec4 out_color;

uniform vec3 user_color;                    
uniform float alpha;

void main(void) {
    out_color = vec4(user_color, alpha);
}
