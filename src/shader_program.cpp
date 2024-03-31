#include "shader_program.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

namespace btoleda
{
    shader_program::shader_program(const std::string &vert, const std::string &frag)
    {
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        std::string vert_source = q_read_file(vert);
        std::string frag_source = q_read_file(frag);

        const char *vert_cstr = vert_source.c_str();
        const char *frag_cstr = frag_source.c_str();

        glShaderSource(vertex_shader, 1, &vert_cstr, nullptr);
        glShaderSource(fragment_shader, 1, &frag_cstr, nullptr);

        glCompileShader(vertex_shader);
        glCompileShader(fragment_shader);

        m_program_id = glCreateProgram();
        glAttachShader(m_program_id, vertex_shader);
        glAttachShader(m_program_id, fragment_shader);
        glLinkProgram(m_program_id);

        // check for linking errors
        int success;
        char infoLog[512];
        glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_program_id, 512, NULL, infoLog);
            std::cerr << "Shader program linking failed...\n"
                      << infoLog << std::endl;
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    shader_program::~shader_program()
    {
        glDeleteProgram(m_program_id);
    }

    shader_program::operator GLuint() const
    {
        return m_program_id;
    }

    // Method from the example capsules
    std::string shader_program::q_read_file(const std::string &filepath)
    {
#if __cplusplus >= 201703L
        if (!std::filesystem::exists(filepath))
        {
            std::stringstream error;
            error << "File read error: " << std::filesystem::current_path() / filepath << " not found.";
            throw std::runtime_error(error.str());
        }
#endif

        std::string contents;
        std::ifstream infile(filepath, std::ios::ate);

#if __cplusplus < 201703L
        if (!infile.is_open())
        {
            std::stringstream error;
            error << "File read error: " << filepath << " not found.";
            throw std::runtime_error(error.str());
        }
#endif

        std::ios::pos_type end = infile.tellg();
        contents.resize(end);
        infile.seekg(0, std::ios::beg);
        infile.read(&contents[0], end);
        infile.close();

        return contents;
    }

    void shader_program::set_uniform(uniform_type t, const std::string& name, void *value)
    {
        auto location = glGetUniformLocation(m_program_id, name.c_str());
        if (location < 0)
        {
            throw std::runtime_error("ERROR::COULD NOT SET SHADER UNIFORM");
        }

        switch (t)
        {
        case uniform_type::MAT4:
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(*(glm::mat4*)value));
            return;

        case uniform_type::INT:
            glUniform1i(location, (int)value);
            return;
        }
    }

}