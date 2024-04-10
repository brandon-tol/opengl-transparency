#version 330 core

out vec2 tex_coord;

layout (location = 0) in vec3 a_position; 

void main() {
    gl_Position = vec4(a_position, 1.0);
    tex_coord = vec2((1.0f + a_position.x) * 0.5f, (1.0f + a_position.y) * 0.5f);

}