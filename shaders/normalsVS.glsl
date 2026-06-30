#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

layout(std140, binding = 0) uniform Matrices{
	mat4 view;
	mat4 projection;
};

uniform mat4 model;
uniform mat3 normalMat;

void main(){
    gl_Position = view * model * vec4(aPos, 1.0); 
    vs_out.normal = normalize(normalMat * aNormal);
}