#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 model;

out VS_OUT{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoords;
} vs_out;


layout(std140, binding = 0) uniform Matrices{
	mat4 view;
	mat4 projection;
};
 
 void main(){
	gl_Position = projection * view * model * vec4(aPos, 1);

	// change later to use normalMat as an instance attribute for better performance
	vs_out.normal = mat3(transpose(inverse(view * model))) * aNormal;
	
	vs_out.fragPos = vec3(view * model * vec4(aPos, 1.0));
	vs_out.texCoords = aTexCoords;
 }