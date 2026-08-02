#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoords;
	vec4 fragPosDirLightSpace;
} vs_out;

uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 dirLightSpaceMat;

layout(std140, binding = 0) uniform Matrices{
	mat4 view;
	mat4 projection;
};
 
 void main(){
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vs_out.normal = normalMat * aNormal;
	vs_out.fragPos = vec3(view * model * vec4(aPos, 1.0));
	vs_out.texCoords = aTexCoords;
	vs_out.fragPosDirLightSpace = dirLightSpaceMat * model * vec4(aPos, 1.0);
 }