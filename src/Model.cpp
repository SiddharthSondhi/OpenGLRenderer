#include "Model.h"
#include "Mesh.h"
#include "Utils.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

Model::Model(std::string path, bool flipUVs) {
	Assimp::Importer import;
    unsigned int flags{ aiProcess_Triangulate };
    if (flipUVs) flags |= aiProcess_FlipUVs;
	const aiScene* scene = import.ReadFile(path, flags);
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

    directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all the node's meshes (if any)
    for (unsigned int i{ 0 }; i < node->mNumMeshes; i++) {
        aiMesh* mesh{ scene->mMeshes[node->mMeshes[i]] };
        meshes.push_back(processMesh(mesh, scene));
    }

    // then do the same for each of its children
    for (unsigned int i{ 0 }; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    std::vector<unsigned int> attribSizes{ 3, 3, 2 };

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
        }
        else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    // process indices
    for (unsigned int i{ 0 }; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j{ 0 }; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    return Mesh(vertices, attribSizes, textures, indices);
}


std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type) {
    
    // loop through each textureName for this type in mat and load the textureName, then create Texture objects and return them
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string textureName{ str.C_Str() };

        //check to see if textureName has already been loaded
        if (loadedTextures.contains(textureName)) {
            textures.push_back(loadedTextures[textureName]);
            continue;
        }

        Texture texture;
        texture.id = Utils::loadTexture(directory + "/" + textureName);
        
        switch (type) {
        case aiTextureType_DIFFUSE:  
            texture.type = Texture::diffuse;
            break;

        case aiTextureType_SPECULAR: 
            texture.type = Texture::specular;
            break;
        }

        textures.push_back(texture);
        loadedTextures[textureName] = texture;
    }

    return textures;
}


void Model::draw(Shader& shader) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].draw(shader);
    }
}