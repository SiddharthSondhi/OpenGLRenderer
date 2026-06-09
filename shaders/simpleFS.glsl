#version 460 core
in vec4 vertexColor;
uniform vec4 ourColor;

out vec4 FragColor;
        
void main(){
    FragColor = vertexColor + 0.2 * ourColor;
};