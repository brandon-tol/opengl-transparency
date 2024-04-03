#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec4 a_color;

out vec3 v_color;

uniform mat4 u_modelview;
uniform mat4 u_perspective;

void main() {
    gl_Position = u_perspective * u_modelview * vec4(a_position, 1.0);
    v_color = a_color.rgb;
}