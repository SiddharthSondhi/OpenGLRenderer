#pragma once

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <vector>

namespace VertexData {

    inline const std::vector<float> cube{
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

    inline const std::vector<float> insideOutCube = {
        // positions          // normals           // texcoords

        // back face (normal points +Z)
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,

         1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        // front face (normal points -Z)
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,

         1.0f,  1.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

        // left face (normal points +X)
        -1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

        -1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

        // right face (normal points -X)
         1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

         1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         // bottom face (normal points +Y)
         -1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
          1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
          1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,

          1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         -1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         -1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,

         // top face (normal points -Y)
         -1.0f,  1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
          1.0f,  1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
          1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,

          1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         -1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         -1.0f,  1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f
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


    inline std::vector<float> genSphereVertices() {
        std::vector<float> vertices;

        constexpr unsigned int X_SEGMENTS{ 64 };
        constexpr unsigned int Y_SEGMENTS{ 64 };
        constexpr float PI{ glm::pi<float>() };

        for (unsigned int y{ 0 }; y <= Y_SEGMENTS; y++) {
            for (unsigned int x{ 0 }; x <= X_SEGMENTS; x++) {

                float u{ static_cast<float>(x) / X_SEGMENTS };
                float v{ static_cast<float>(y) / Y_SEGMENTS };

                float theta{ u * 2.0f * PI };
                float phi{ v * PI };

                float xPos{ std::cos(theta) * std::sin(phi) };
                float yPos{ std::cos(phi) };
                float zPos{ std::sin(theta) * std::sin(phi) };

                // Position
                glm::vec3 position{
                    xPos,
                    yPos,
                    zPos
                };

                // Unit sphere centered at origin:
                // normal == position
                glm::vec3 normal{
                    xPos,
                    yPos,
                    zPos
                };

                // Direction of increasing U around the sphere
                glm::vec3 tangent{
                    -std::sin(theta),
                    0.0f,
                    std::cos(theta)
                };

                tangent = glm::normalize(tangent);

                // Complete the tangent-space basis
                glm::vec3 bitangent{
                    glm::normalize(glm::cross(normal, tangent))
                };

                // Position
                vertices.push_back(position.x);
                vertices.push_back(position.y);
                vertices.push_back(position.z);

                // Normal
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);

                // Texture coordinates
                vertices.push_back(u);
                vertices.push_back(v);

                // Tangent
                vertices.push_back(tangent.x);
                vertices.push_back(tangent.y);
                vertices.push_back(tangent.z);

                // Bitangent
                vertices.push_back(bitangent.x);
                vertices.push_back(bitangent.y);
                vertices.push_back(bitangent.z);
            }
        }

        return vertices;
    }

    inline std::vector<unsigned int> genSphereIndices() {
        std::vector<unsigned int> indices;

        constexpr unsigned int X_SEGMENTS{ 64 };
        constexpr unsigned int Y_SEGMENTS{ 64 };

        for (unsigned int y{ 0 }; y < Y_SEGMENTS; y++){
            for (unsigned int x{ 0 }; x < X_SEGMENTS; x++){
                unsigned int topLeft{ y * (X_SEGMENTS + 1) + x };
                unsigned int bottomLeft{ (y + 1) * (X_SEGMENTS + 1) + x };
                unsigned int topRight{ topLeft + 1 };
                unsigned int bottomRight{ bottomLeft + 1 };

                indices.push_back(topLeft);
                indices.push_back(topRight);
                indices.push_back(bottomLeft);

                indices.push_back(topRight);
                indices.push_back(bottomRight);
                indices.push_back(bottomLeft);
            }
        }

        return indices;
    }
}