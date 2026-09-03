#pragma once
#include <glm/glm.hpp>
#include <string_view>
#include <string>

class Shader {
public:
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	void use() const;
	unsigned int getID() const;

	void setBool(const std::string name, bool value) const;
	void setInt(const std::string name, int value) const;
	void setFloat(const std::string name, float value) const;
	void setVec2(const std::string name, const glm::vec2& value) const;
	void setVec3(const std::string name, const glm::vec3& value) const;
	void setMat3(const std::string name, const glm::mat3& value) const;
	void setMat4(const std::string name, const glm::mat4& value) const;

private:
	unsigned int ID;
	void checkCompileErrors(unsigned int shader, std::string_view type);

};