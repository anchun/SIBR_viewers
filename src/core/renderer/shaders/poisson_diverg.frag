#version 420

layout(binding = 0) uniform sampler2D synth;
layout(location= 0) out vec4 out_constraint;

void main(void) {
    vec4 I = texelFetch(synth, ivec2(gl_FragCoord.xy), 0);

    // hole - perform Poisson synthesis here
    if (all(lessThan(I.xyz,vec3(0.01))))
        out_constraint = vec4(0);
    else
        out_constraint = I;
}
