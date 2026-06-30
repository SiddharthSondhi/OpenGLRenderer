#version 460 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

uniform samplerCube skybox;

void main(){
	// IN VIEW SPACE
	//vector from cameraPos to fragPos
	vec3 I = normalize(fragPos); 

	float ratio = 1.0 / 1.52;
	vec3 R = refract(I, normalize(normal), ratio);

	fragColor =  texture(skybox, R);	
}