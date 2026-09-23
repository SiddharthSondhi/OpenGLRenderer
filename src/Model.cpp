#include "Model.h"
#include "Mesh.h"
#include "Utils.h"
#include "PhongMaterial.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

Model::Model(std::string path, const ModelLoadOptions& options) {
    unsigned int flags{ aiProcess_Triangulate };
    if (options.flipUVs) flags |= aiProcess_FlipUVs;
    if (options.genSmoothNormals) flags |= aiProcess_GenSmoothNormals;
    if (options.calcTangentSpace) flags |= aiProcess_CalcTangentSpace;

    Assimp::Importer import;
    std::cout << "Loading model: " << path << '\n';
	const aiScene* scene = import.ReadFile(path, flags);
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

    directory = path.substr(0, path.find_last_of('/'));
    processMaterials(scene, options);
	processNode(scene->mRootNode, scene, options);
}

Model::Model(const Mesh& mesh, const Material& material) {
    meshes.push_back(mesh);
    materials.push_back(material.clone());
}


void Model::processMaterials(const aiScene* scene, const ModelLoadOptions& options) {
    materials.reserve(scene->mNumMaterials);

    for (unsigned int i{ 0 }; i < scene->mNumMaterials; i++) {
        aiMaterial* assimpMat{ scene->mMaterials[i] };

        unsigned int diffuseTex = loadMaterialTextures(assimpMat, aiTextureType_DIFFUSE, scene);
        unsigned int specularTex = loadMaterialTextures(assimpMat, aiTextureType_SPECULAR, scene);
        unsigned int normalTex = loadMaterialTextures(assimpMat, aiTextureType_HEIGHT, scene);
        unsigned int emissionTex = loadMaterialTextures(assimpMat, aiTextureType_EMISSIVE, scene);

        aiColor3D diffColor{ 1.0f, 1.0f, 1.0f };
        assimpMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffColor);

        aiColor3D specColor{ 1.0f, 1.0f, 1.0f };
        assimpMat->Get(AI_MATKEY_COLOR_SPECULAR, specColor);

        aiColor3D emissiveColor;
        // if no emission texture, then the default value will be 0, otherwise 1 (bc it is used to scale the texture)
        if (emissionTex == 0)
            emissiveColor = { 0, 0, 0 };
        else
            emissiveColor = { 1, 1, 1 };

        assimpMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);

        materials.emplace_back(std::make_unique<PhongMaterial>
            (diffuseTex, specularTex, normalTex, emissionTex,
            glm::vec3{ diffColor.r, diffColor.g, diffColor.g }, 
            glm::vec3{ specColor.r, specColor.g, specColor.b },
            glm::vec3{ emissiveColor.r, emissiveColor.g, emissiveColor.b }
        ));

        if (options.printDebugInfo) {
            std::cout << "Material# " << i << ":\n";
            printMaterialInfo(assimpMat);
            std::cout << "\n";
        }
    }
}

void Model::processNode(aiNode* node, const aiScene* scene, const ModelLoadOptions& options) {
    // process all the node's meshes (if any)
    for (unsigned int i{ 0 }; i < node->mNumMeshes; i++) {
        aiMesh* mesh{ scene->mMeshes[node->mMeshes[i]] };
        meshes.push_back(processMesh(mesh, scene, options));
    }

    // then do the same for each of its children
    for (unsigned int i{ 0 }; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, options);
    }
}


Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const ModelLoadOptions& options) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> attribSizes{ 3, 3, 2 };
    if (options.calcTangentSpace) {
        attribSizes.push_back(3);
        attribSizes.push_back(3);
    }

    //process vertices
    for (unsigned int i{ 0 }; i < mesh->mNumVertices; i++) {
        // positions
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        // normals
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);

        // textureName coordinates
        if (mesh->mTextureCoords[0]) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
            
            if (options.calcTangentSpace) {
                //tangent
                vertices.push_back(mesh->mTangents[i].x);
                vertices.push_back(mesh->mTangents[i].y);
                vertices.push_back(mesh->mTangents[i].z);

                //bitangent
                vertices.push_back(mesh->mBitangents[i].x);
                vertices.push_back(mesh->mBitangents[i].y);
                vertices.push_back(mesh->mBitangents[i].z);
            }
        }

        // if no texture coordinates found then just use 0 
        else {
            for (int i{ 0 }; i < 2; i++) {
                vertices.push_back(0.0f);
            }
            if (options.calcTangentSpace) {
                for (int i{ 0 }; i < 6; i++) {
                    vertices.push_back(0.0f);
                }
            }
        }
    }

    // process indices
    for (unsigned int i{ 0 }; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j{ 0 }; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    return Mesh(vertices, attribSizes, mesh->mMaterialIndex, indices);
}



unsigned int Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene) {
    if (mat->GetTextureCount(type) == 0) 
        return 0;

    if (mat->GetTextureCount(type) > 1) {
        std::cout << "Material contains " << mat->GetTextureCount(type) << " textures of type " << type << "; only using the first.\n";
    }

    aiString str;
    mat->GetTexture(type, 0, &str);
    std::string textureName{ str.C_Str() };

    if (textureName.empty()) {
        return 0;
    }

    //check to see if textureName has already been loaded
    if (loadedTextures.contains(textureName)) {
        return loadedTextures[textureName];
    }

    bool gammaCorrected{ false };

    switch (type) {
    case aiTextureType_DIFFUSE:
        gammaCorrected = true;
        break;
    }

    unsigned int texture;

    // if texture is embedded
    if (textureName[0] == '*'){
        unsigned int textureIndex{ static_cast<unsigned int>(std::stoi(textureName.substr(1)))};

        if (textureIndex < scene->mNumTextures){
            aiTexture* embeddedTexture = scene->mTextures[textureIndex];

            texture = Utils::loadTextureFromMemory( reinterpret_cast<unsigned char*>(embeddedTexture->pcData), embeddedTexture->mWidth, gammaCorrected);
        }
        else{
            std::cout << "Invalid embedded texture index: " << textureName << '\n';
            return 0;
        }
    }
    // otherwise load from file
    else{
        texture = Utils::loadTextureFromFile(directory + "/" + textureName, gammaCorrected);
    }
        
    loadedTextures[textureName] = texture;
    return texture;
}


const std::vector<Mesh>& Model::getMeshes() const {
    return meshes;
}

const std::vector<std::unique_ptr<Material>>& Model::getMaterials() const {
    return materials;
}

void Model::printMaterialInfo(const aiMaterial* mat) const {
    // print base material colors
    std::cout << "Material Base Colors:\n";
    aiColor3D color;

    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        std::cout << "Diffuse color: "
            << color.r << ", "
            << color.g << ", "
            << color.b << '\n';
    }

    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        std::cout << "Specular color: "
            << color.r << ", "
            << color.g << ", "
            << color.b << '\n';
    }

    if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
        std::cout << "Emissive color: "
            << color.r << ", "
            << color.g << ", "
            << color.b << '\n';
    }

    // print material opacity
    float opacity;
    if (mat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
        std::cout << "Opacity: " << opacity << '\n';
    }

    // print material shininess
    float shininess;
    if (mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
        std::cout << "Shininess: " << shininess << '\n';
    }

    // print material shininess
    float shininessStrength;
    if (mat->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength) == AI_SUCCESS) {
        std::cout << "Shininess strength: " << shininessStrength << '\n';
    }

    std::cout << "\nMaterial Textures:\n";

    // print material textures
    aiString path;
    std::cout << "Diffuse: " << mat->GetTextureCount(aiTextureType_DIFFUSE);
    if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        std::cout << " Path: " << path.C_Str();
    std::cout << "\n";

    std::cout << "BaseColor: " << mat->GetTextureCount(aiTextureType_BASE_COLOR);
    if (mat->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == AI_SUCCESS)
        std::cout << " Path: " << path.C_Str();
    std::cout << "\n";

    std::cout << "Specular: " << mat->GetTextureCount(aiTextureType_SPECULAR);
    if (mat->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
        std::cout << " Path: " << path.C_Str();
    std::cout << "\n";

    std::cout << "Normals: " << mat->GetTextureCount(aiTextureType_NORMALS);
    if (mat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
        std::cout << " Path: " << path.C_Str();
    std::cout << "\n";

    std::cout << "Metalness: " << mat->GetTextureCount(aiTextureType_METALNESS);
    if (mat->GetTexture(aiTextureType_METALNESS, 0, &path) == AI_SUCCESS)
        std::cout << " Path: " << path.C_Str();
    std::cout << "\n";

    std::cout << "Roughness: " << mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
    if (mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == AI_SUCCESS)
        std::cout << " Path: " << path.C_Str();
    std::cout << "\n";

    std::cout << "Ambient Occlusion: " << mat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
    if (mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path) == AI_SUCCESS)
        std::cout << " Path: " << path.C_Str();
    std::cout << "\n";

    std::cout << "Emissive: " << mat->GetTextureCount(aiTextureType_EMISSIVE);
    if (mat->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS)
        std::cout << " Path: " << path.C_Str();
    std::cout << "\n";
}



