#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT{
    vec3 fragPos;
    vec2 texCoords;
	vec3 normal;
	mat3 TBN;
} vs_out;

layout(std140, binding = 0) uniform Matrices{
	mat4 view;
	mat4 projection;
};

uniform mat4 model;
uniform mat3 normalMat;

void main(){
	vec4 viewWorldPos = view * model * vec4(aPos, 1.0);
	vs_out.fragPos = viewWorldPos.xyz;
	vs_out.texCoords = aTexCoords;
	vs_out.normal = normalMat * aNormal;

	vec3 T = normalize(vec3(view * model * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(view * model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(view * model * vec4(aNormal,    0.0)));
    vs_out.TBN = mat3(T, B, N);

	gl_Position = projection * viewWorldPos;
}