#version 330 core

out vec2 v_tex_coords;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_tex_coords;

void main()
{
    gl_Position = vec4(a_position, 0.0, 1.0);
    v_tex_coords = a_tex_coords;
}

