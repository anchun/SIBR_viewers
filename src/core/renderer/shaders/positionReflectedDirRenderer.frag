#version 420

in vec3 position;
in vec3 normal;

uniform vec3 cameraPos;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outDirection;

void main(void) {
	outPosition = position;
	outDirection = reflect(normalize(position - cameraPos), normal);
}

