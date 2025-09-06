#include <util/shader.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>

namespace util
{

Shader::Shader(const char* vertex_path, const char* fragment_path)
    : ID{ 0 }
    , error{ true }
{
    GLuint vertex_shader;
    if (!compile_shader(vertex_path, GL_VERTEX_SHADER, vertex_shader))
    {
        return;
    }

    GLuint fragment_shader;
    if (!compile_shader(fragment_path, GL_FRAGMENT_SHADER, fragment_shader))
    {
        return;
    }

    error = !build_program(vertex_shader, fragment_shader);

    // Can delete the shaders now.
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::~Shader()
{
    if (error)
        return;
    glDeleteProgram(ID);
}

void Shader::use()
{
    if (error)
        return;
    glUseProgram(ID);
}

void Shader::set_bool(const std::string& name, bool value) const
{
    if (error)
        return;
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::set_int(const std::string& name, int value) const
{
    if (error)
        return;
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_float(const std::string& name, float value) const
{
    if (error)
        return;
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// Assumes the matrix is stored in column major layout.
void Shader::set_matrix4f(const std::string& name, const glm::mat4& mat4) const
{
    if (error)
        return;
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::set_vec4(const std::string& name, const glm::vec4& vec4) const
{
    if (error)
        return;
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec4));
}

// static
bool Shader::load_shader(const std::string& fname, std::string& buffer)
{
    std::ifstream fin(fname);
    if (!fin)
    {
        std::cerr << "[ERROR] Error opening shader file " << fname << '\n';
        return false;
    }
    try
    {
        fin.seekg(0, std::ios::end);
        buffer.reserve(fin.tellg());
        fin.seekg(0, std::ios::beg);
        buffer.assign((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        fin.close();
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "[ERROR] Error reading shader file " << fname << '\n';
        return false;
    }
    return true;
}

// static
bool Shader::compile_shader(const char* shader_path, GLenum shader_type, GLuint& shader_id)
{
    std::string buffer;
    if (!load_shader(shader_path, buffer))
    {
        return false;
    }
    const char* shader_source = buffer.c_str();
    shader_id = glCreateShader(shader_type);
    // second param is the number of strings passed.
    glShaderSource(shader_id, 1, &shader_source, NULL);
    glCompileShader(shader_id);
    // Check for compile errors:
    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (success)
    {
        return true;
    }
    char info_log[512];
    // Compilation failed. Show the errors and bail out.
    glGetShaderInfoLog(shader_id, sizeof(info_log), NULL, info_log);
    std::cerr << "[ERROR] " << (shader_type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
              << " Shader compilation failed:\n"
              << info_log << '\n';
    return false;
}

bool Shader::build_program(GLuint vertex_shader, GLuint fragment_shader)
{
    char info_log[1024];
    ID = glCreateProgram();
    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);
    glLinkProgram(ID);
    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, sizeof(info_log), NULL, info_log);
        std::cerr << "[ERROR] Program link failed!\n" << info_log << '\n';
        return false;
    }

    glValidateProgram(ID);
    glGetProgramiv(ID, GL_VALIDATE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, sizeof(info_log), NULL, info_log);
        std::cerr << "[ERROR] Program validation failed!\n" << info_log << '\n';
        return false;
    }

    return true;
}

}
