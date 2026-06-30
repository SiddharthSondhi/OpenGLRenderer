#pragma once

#include <glad/glad.h>
#include <vector>

#include "Shader.h"
#include "Camera.h"


class Skybox{
public:
	unsigned int texture; //cubemap texture ID

	Skybox(const std::vector<float>& vertices, unsigned int texture);
	void draw(Shader& shader);


private:
	unsigned int VBO, VAO;
	

};