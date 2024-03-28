#version 330 core

in vec4 v_color;

out vec4 color;

void main()
{
    color = vec4(vec3(v_color), 0.5);
}