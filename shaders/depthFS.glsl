#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

float near = 0.1; 
float far  = 300.0; 
uniform sampler2D texture_diffuse1;

  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC from range [0 -1] to [-1 to 1]
    return (2.0 * near * far) / (far + near - z * (far - near)); //inverse of ndc transformation to get linear depth val
}

void main()
{   
    // values range from near to far. divide by far to get values between [0, 1]
    // 1 - val to invert black / white values
    float depth = 1 - (LinearizeDepth(gl_FragCoord.z) / (far * .25)); 
    //vec4 texVal = texture(texture_diffuse1, TexCoords);
    //FragColor = vec4(texVal.x, texVal.y, depth, 1.0);

    FragColor = vec4(vec3(depth), 1.0f);
}