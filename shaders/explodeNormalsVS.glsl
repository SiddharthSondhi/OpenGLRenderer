#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec2 texCoords;
} vs_out;

uniform mat4 model;
uniform mat3 normalMat;

layout(std140, binding = 0) uniform Matrices{
	mat4 view;
	mat4 projection;
};
 
 void main(){
	 vs_out.texCoords = aTexCoords;
     gl_Position = projection * view * model * vec4(aPos, 1.0); 
 }