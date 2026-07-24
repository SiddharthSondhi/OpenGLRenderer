#pragma once

#include <glad/glad.h>

class Texture {
public:
	enum Type {
		diffuse,
		specular
	};

	unsigned int id{ 0 };
	Type type;

	void setTextureWrapMode(GLenum textureWrapMode) const;
};