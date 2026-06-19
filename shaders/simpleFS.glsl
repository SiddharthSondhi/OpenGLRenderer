#version 460 core
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

out vec4 FragColor;
        
void main(){
    FragColor = mix(texture(texture1, texCoord) * 2.5, texture(texture2, texCoord), 0.8) ;
};