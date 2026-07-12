#pragma once

#include <vector>

namespace VertexData {
    inline const std::vector<float> cubeNormalsTexture{
        // Back (-Z)
        -0.5f,-0.5f,-0.5f,  0,0,-1,  0,0,
        -0.5f, 0.5f,-0.5f,  0,0,-1,  0,1,
         0.5f, 0.5f,-0.5f,  0,0,-1,  1,1,

         0.5f, 0.5f,-0.5f,  0,0,-1,  1,1,
         0.5f,-0.5f,-0.5f,  0,0,-1,  1,0,
        -0.5f,-0.5f,-0.5f,  0,0,-1,  0,0,

        // Front (+Z)
        -0.5f,-0.5f, 0.5f,  0,0,1,   0,0,
         0.5f,-0.5f, 0.5f,  0,0,1,   1,0,
         0.5f, 0.5f, 0.5f,  0,0,1,   1,1,

         0.5f, 0.5f, 0.5f,  0,0,1,   1,1,
        -0.5f, 0.5f, 0.5f,  0,0,1,   0,1,
        -0.5f,-0.5f, 0.5f,  0,0,1,   0,0,

        // Left (-X)
        -0.5f,-0.5f,-0.5f, -1,0,0,   0,0,
        -0.5f,-0.5f, 0.5f, -1,0,0,   1,0,
        -0.5f, 0.5f, 0.5f, -1,0,0,   1,1,

        -0.5f, 0.5f, 0.5f, -1,0,0,   1,1,
        -0.5f, 0.5f,-0.5f, -1,0,0,   0,1,
        -0.5f,-0.5f,-0.5f, -1,0,0,   0,0,

        // Right (+X)
         0.5f,-0.5f,-0.5f,  1,0,0,   0,0,
         0.5f, 0.5f,-0.5f,  1,0,0,   0,1,
         0.5f, 0.5f, 0.5f,  1,0,0,   1,1,

         0.5f, 0.5f, 0.5f,  1,0,0,   1,1,
         0.5f,-0.5f, 0.5f,  1,0,0,   1,0,
         0.5f,-0.5f,-0.5f,  1,0,0,   0,0,

         // Bottom (-Y)
         -0.5f,-0.5f,-0.5f,  0,-1,0,  0,0,
          0.5f,-0.5f,-0.5f,  0,-1,0,  1,0,
          0.5f,-0.5f, 0.5f,  0,-1,0,  1,1,

          0.5f,-0.5f, 0.5f,  0,-1,0,  1,1,
         -0.5f,-0.5f, 0.5f,  0,-1,0,  0,1,
         -0.5f,-0.5f,-0.5f,  0,-1,0,  0,0,

         // Top (+Y)
         -0.5f, 0.5f,-0.5f,  0,1,0,   0,0,
         -0.5f, 0.5f, 0.5f,  0,1,0,   0,1,
          0.5f, 0.5f, 0.5f,  0,1,0,   1,1,

          0.5f, 0.5f, 0.5f,  0,1,0,   1,1,
          0.5f, 0.5f,-0.5f,  0,1,0,   1,0,
         -0.5f, 0.5f,-0.5f,  0,1,0,   0,0
    };


    inline const std::vector<float> cubeTex  {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
         // Bottom face
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
         // Top face
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left  
    };

    inline const std::vector<float> plane{
        // positions            // normals         // tex coords
         5.0f, -0.5f,  5.0f,    0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,    0.0f, 1.0f, 0.0f,  0.0f, 2.0f,
        -5.0f, -0.5f,  5.0f,    0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

         5.0f, -0.5f,  5.0f,    0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
         5.0f, -0.5f, -5.0f,    0.0f, 1.0f, 0.0f,  2.0f, 2.0f,
        -5.0f, -0.5f, -5.0f,    0.0f, 1.0f, 0.0f,  0.0f, 2.0f,
    };

    inline const std::vector<float> transparent = {
        // positions         // texture Coords
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    inline const std::vector<float> screenQuad = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    inline const std::vector<float> skyboxVertices = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

}