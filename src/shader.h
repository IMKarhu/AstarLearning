#pragma once
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader
{
public:
    Shader(const char* vertex, const char* fragment);
    ~Shader();
    void use();

    void setUniformVec4(const std::string& name, const glm::vec4& vec4);
    void setUniformVec3(const std::string& name, const glm::vec3& vec3);
    void setUniformMat4(const std::string& name, const glm::mat4& mat4);
    void setUniformTexture(const std::string& name, int value);
    void setUniformInt(const std::string& name, const int& value);
    void setUniformFloat(const std::string& name, const float& value);
private:
    unsigned int m_shaderProgram = 0;
    unsigned int m_vertexShader = 0;
    unsigned int m_fragmentShader = 0;
};