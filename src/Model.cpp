#include "Model.h"
#include "Mesh.h"
#include "Utils.h"
#include "PhongMaterial.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

Model::Model(std::string path, const Shader& shader, const ModelLoadOptions& options) {
    unsigned int flags{ aiProcess_Triangulate};
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
    processMaterials(scene, shader);
	processNode(scene->mRootNode, scene, options);
}

Model::Model(const Mesh& mesh, const Material& material) {
    meshes.push_back(mesh);
    materials.push_back(material.clone());
}


void Model::processMaterials(const aiScene* scene, const Shader& shader) {
    materials.reserve(scene->mNumMaterials);

    for (unsigned int i{ 0 }; i < scene->mNumMaterials; i++) {
        aiMaterial* assimpMat{ scene->mMaterials[i] };

        std::vector<Texture> textures;

        std::vector<Texture> diffuseMaps = loadMaterialTextures(assimpMat, aiTextureType_DIFFUSE, scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(assimpMat, aiTextureType_SPECULAR, scene);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = loadMaterialTextures(assimpMat, aiTextureType_HEIGHT, scene);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        float shininess = 128.0f;
        assimpMat->Get(AI_MATKEY_SHININESS, shininess);

        materials.emplace_back(std::make_unique<PhongMaterial>(shader, std::move(textures), shininess));
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
    std::vector<Texture> textures;
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



std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene) {
    
    // loop through each textureName for this type in mat and load the textureName, then create Texture objects and return them
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string textureName{ str.C_Str() };

        if (textureName.empty()) {
            continue;
        }

        //check to see if textureName has already been loaded
        if (loadedTextures.contains(textureName)) {
            textures.push_back(loadedTextures[textureName]);
            continue;
        }

        Texture texture;
        bool gammaCorrected{ false };

        switch (type) {
        case aiTextureType_DIFFUSE:
            texture.type = Texture::diffuse;
            gammaCorrected = true;
            break;
        case aiTextureType_SPECULAR:
            texture.type = Texture::specular;
            break;
        case aiTextureType_HEIGHT:
            texture.type = Texture::normal;
            break;
        }

        // if texture is embedded
        if (textureName[0] == '*'){
            unsigned int textureIndex{ static_cast<unsigned int>(std::stoi(textureName.substr(1)))};

            if (textureIndex < scene->mNumTextures){
                aiTexture* embeddedTexture = scene->mTextures[textureIndex];

                texture.id = Utils::loadTextureFromMemory(
                    reinterpret_cast<unsigned char*>(embeddedTexture->pcData), 
                    embeddedTexture->mWidth, 
                    gammaCorrected);
            }
            else
            {
                std::cout << "Invalid embedded texture index: " << textureName << '\n';
                continue;
            }
        }
        // otherwise load from file
        else{
            texture.id = Utils::loadTextureFromFile(directory + "/" + textureName, gammaCorrected);
        }
        
        textures.push_back(texture);
        loadedTextures[textureName] = texture;
    }

    return textures;
}


const std::vector<Mesh>& Model::getMeshes() const {
    return meshes;
}

const std::vector<std::unique_ptr<Material>>& Model::getMaterials() const {
    return materials;
}



