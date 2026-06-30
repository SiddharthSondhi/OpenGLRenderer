#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat3 normalMat;

layout(std140, binding = 0) uniform Matrices{
	mat4 view;
	mat4 projection;
};

void main(){
	normal = normalMat * aNormal;
	fragPos = vec3(view * model * vec4(aPos, 1.0));
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
