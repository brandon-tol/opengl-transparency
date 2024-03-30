#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

out float v_z_index;
out vec4 v_color;

uniform mat4 modelview;
uniform mat4 perspective;

void main()
{
    gl_Position = perspective * modelview * vec4(position, 1.0);
    v_color = color;
    v_z_index = position.z;

}