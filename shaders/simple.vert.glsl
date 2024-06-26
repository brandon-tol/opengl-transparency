#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;

out vec2 v_tex_coord;

uniform mat4 u_modelview;
uniform mat4 u_perspective;

void main() {
    gl_Position = u_perspective * u_modelview * vec4(a_position, 1.0);
    v_tex_coord = a_tex_coord;
}