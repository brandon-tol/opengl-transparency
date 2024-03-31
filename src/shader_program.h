#pragma once
#include <glew.h>
#include <string>

namespace btoleda
{
    enum class uniform_type
    {
        NONE = 0,
        FLOAT,
        UINT,
        INT,
        VEC2,
        VEC3,
        VEC4,
        MAT2,
        MAT3,
        MAT4
    };

    class shader_program
    {
    public:
        shader_program(const std::string &vert, const std::string &frag);
        ~shader_program();
        operator GLuint() const;
        void set_uniform(uniform_type type, const std::string &name, void *value);

    private:
        GLuint m_program_id;

        std::string q_read_file(const std::string &filepath);
    };
}
