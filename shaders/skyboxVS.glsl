#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

layout(std140, binding = 0) uniform Matrices{
	mat4 view;
	mat4 projection;
};

void main(){
    mat4 viewNoTrans = mat4(mat3(view));
    texCoords = aPos;
    vec4 pos = projection * viewNoTrans * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  