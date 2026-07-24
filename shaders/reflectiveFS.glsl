#version 460 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

uniform samplerCube skybox;

void main(){
	// IN VIEW SPACE
	//vector from cameraPos to fragPos
	vec3 I = normalize(fragPos); 
	vec3 R = reflect(I, normalize(normal));
	fragColor =  texture(skybox, R);
}