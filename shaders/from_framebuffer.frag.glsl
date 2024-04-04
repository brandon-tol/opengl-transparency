#version 330 core

out vec4 f_color;

in vec2 tex_coord;

uniform sampler2D u_fb_texture;

void main() {
    f_color = texture(u_fb_texture, tex_coord);
}