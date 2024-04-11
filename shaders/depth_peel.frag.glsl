#version 330 core

layout (location = 0) out vec4 f_color;

in vec2 v_tex_coord;

uniform sampler2D u_depth;
uniform sampler2D u_texture;
uniform vec2 u_viewport_dimensions;

void main() {
    if (gl_FragCoord.z <= texture(u_depth, gl_FragCoord.xy / u_viewport_dimensions).r)
    {
        discard;
    }
    f_color = texture(u_texture, v_tex_coord);
}