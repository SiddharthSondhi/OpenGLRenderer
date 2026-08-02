#version 460 core

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D depthMap;

void main(){
    // .r because shadowMap has a GL_DEPTH_COMPONENT format which stores depth values as (depth, 0, 0, 1.0) -> (r, g, b, a)
    float depth = texture(depthMap, texCoords).r;
    fragColor = vec4(vec3(depth), 1.0);
}