#version 460 core
in vec2 texCoord;

uniform sampler2D texture_diffuse1;

out vec4 FragColor;
        
void main(){
    vec4 texColor = texture(texture_diffuse1, texCoord);
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor; 
}