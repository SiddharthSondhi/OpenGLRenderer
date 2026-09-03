#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

namespace Utils {
	unsigned int loadTextureFromFile(std::string path, bool gammaCorrected = false);
	unsigned int loadTextureFromMemory(const unsigned char* fileData, unsigned int fileSize, bool gammaCorrected = false);

	unsigned int loadCubemap(std::vector<std::string> faces);
	unsigned int loadCubemap(std::string folder);

	void setTextureWrapMode(unsigned int textureID, GLenum textureWrapMode);
}