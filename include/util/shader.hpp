#pragma once

#include <string>
#include <glad/glad.h>

namespace util
{

class Shader
{
public:
    unsigned int ID;
    bool error;

    // constructor that reads and builds the shader program.
    Shader(const char* vertex_path, const char* fragment_path);
    ~Shader();
    // activate the shader program.
    void use();
    // functions to set values to uniform variables.
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;

private:
    static bool load_shader(const std::string& fname, std::string& buffer);
    static bool compile_shader(const char* shader_path, GLenum shader_type, GLuint& shader_id);
    bool build_program(GLuint vertex_shader, GLuint fragment_shader);
};

}
