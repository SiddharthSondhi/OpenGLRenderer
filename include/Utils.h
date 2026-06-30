#pragma once

#include <string>
#include <vector>

namespace Utils {
	unsigned int loadTextureFromFile(std::string path);
	unsigned int loadTextureFromMemory(const unsigned char* fileData, unsigned int fileSize);

	unsigned int loadCubemap(std::vector<std::string> faces);
	unsigned int loadCubemap(std::string folder);
}