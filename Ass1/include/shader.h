#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader {
public:
    // Program ID
    unsigned int ID;

    // Constructor
    Shader();

    // Load shaders from files
    bool loadFromFile(const char* vertexPath, const char* fragmentPath);

    // Activate the shader
    void use() const;

    // Utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    // Utility function for checking shader compilation/linking errors
    void checkCompileErrors(GLuint shader, std::string type);
};