#include "shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const char* vertex, const char* fragment)
{
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertex);
        fShaderFile.open(fragment);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << e.code() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(m_vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(m_vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(m_vertexShader, 512, nullptr, infoLog);
        std::cout << "Error Shader Vertex compilation failed!\n" << infoLog << std::endl;
    }

    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(m_fragmentShader);

    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(m_fragmentShader, 512, nullptr, infoLog);
        std::cout << "Error Shader fragment compilation failed!\n" << infoLog << std::endl;
    }

    m_shaderProgram = glCreateProgram();

    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glLinkProgram(m_shaderProgram);

    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
    }

    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
}

Shader::~Shader() {}

void Shader::use()
{
    glUseProgram(m_shaderProgram);
}


void Shader::setUniformVec4(const std::string& name, const glm::vec4& vec4)
{
    int location = glGetUniformLocation(m_shaderProgram, name.c_str());
    if (location < 0)
    {
        return;
    }
    glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w);
}
void Shader::setUniformVec3(const std::string& name, const glm::vec3& vec3)
{
    int location = glGetUniformLocation(m_shaderProgram, name.c_str());
    if (location < 0)
    {
        return;
    }
    glUniform3f(location, vec3.x, vec3.y, vec3.z);
}
void Shader::setUniformMat4(const std::string& name, const glm::mat4& mat4)
{
    int location = glGetUniformLocation(m_shaderProgram, name.c_str());
    if (location < 0)
    {
        return;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat4[0][0]);
}
void Shader::setUniformTexture(const std::string& name, int value)
{
    int location = glGetUniformLocation(m_shaderProgram, name.c_str());
    if (location < 0)
    {
        return;
    }
    glUniform1i(location, value);
}
void Shader::setUniformInt(const std::string& name, const int& value)
{
    int location = glGetUniformLocation(m_shaderProgram, name.c_str());
    if (location < 0)
    {
        return;
    }
    glUniform1i(location, value);
}

void Shader::setUniformFloat(const std::string& name, const float& value)
{
    int location = glGetUniformLocation(m_shaderProgram, name.c_str());
    if (location < 0)
    {
        return;
    }
    glUniform1f(location, value);
}
