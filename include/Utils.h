#pragma once

#include <string>

namespace Utils {
	unsigned int loadTextureFromFile(std::string path);
	unsigned int loadTextureFromMemory(const unsigned char* fileData, unsigned int fileSize);
}