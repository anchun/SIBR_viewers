#version 420

layout(location = 0) out float out_color;

void main(void) {
  out_color   = gl_FragCoord.z;
}
