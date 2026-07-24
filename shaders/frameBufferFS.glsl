#version 460 core
out vec4 fragColor;
  
in vec2 texCoords;


#define MODE_REGULAR 0
#define MODE_INVERSE 1
#define MODE_GREY_SCALE 2
#define MODE_GREY_SCALE_WEIGHTED 3
#define MODE_SHARPEN 4
#define MODE_EMBOSS 5
#define MODE_TEST 6

uniform sampler2D screenTexture; //screen texture
uniform int postProcessingMode = MODE_REGULAR;
uniform float offset  = 1.0f / 300.0f;


void main(){
    // NOT USING conv matrix
    if (postProcessingMode == MODE_REGULAR){ 
        fragColor = texture(screenTexture, texCoords);
    }
    else if (postProcessingMode == MODE_INVERSE){
        fragColor =  vec4(vec3(1.0 - texture(screenTexture, texCoords)), 1.0);
    }
    else if (postProcessingMode == MODE_GREY_SCALE){
        fragColor = texture(screenTexture, texCoords);
        float average = (fragColor.r + fragColor.g + fragColor.b) / 3.0f;
        fragColor = vec4(average, average, average, 1.0f);
    }
    else if (postProcessingMode == MODE_GREY_SCALE_WEIGHTED){
        fragColor = texture(screenTexture, texCoords);
        float average = 0.2126f * fragColor.r + 0.7152f * fragColor.g + 0.0722f * fragColor.b;
        fragColor = vec4(average, average, average, 1.0f);
    }

    // USING conv matrix
    else{    
        // define offsets (how far from the surrounding pixel to measure)
        vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
        );

        // default kernel
        float kernel[9] = float[](
            0, 0, 0,
            0, 1, 0,
            0, 0, 0
        );
        
        // kernel options
        if (postProcessingMode == MODE_SHARPEN){
            kernel = float[](
                -2, -1, -2,
                -1,  13, -1,
                -2, -1, -2
            );
        }
        else if (postProcessingMode == MODE_EMBOSS){
                kernel = float[](
                    -2, -1,  0,
                    -1,  1,  1,
                     0,  1,  2
                );
        }

        else if (postProcessingMode == MODE_TEST){
                kernel = float[](
                    1.0f/9.0f,  1.0f/9.0f,  1.0f/9.0f,
                    1.0f/9.0f,  1.0f/9.0f,  1.0f/9.0f,
                    1.0f/9.0f,  1.0f/9.0f,  1.0f/9.0f
                );
        }
            
        
        // apply convolutional matrix
        vec3 sampleTex[9];
    
        for(int i = 0; i < 9; i++){
            sampleTex[i] = vec3(texture(screenTexture, texCoords + offsets[i]));
        }

        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];
        
        fragColor = vec4(col, 1.0);
    }

    // apply gamma correction
    float gamma = 2.2;
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
}
    