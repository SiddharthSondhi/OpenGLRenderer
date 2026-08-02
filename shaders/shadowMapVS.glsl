#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 dirLightSpaceMat;
uniform mat4 model;

void main(){
	gl_Position = dirLightSpaceMat * model * vec4(aPos, 1.0);
}