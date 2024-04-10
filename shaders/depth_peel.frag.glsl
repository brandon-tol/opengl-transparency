#version 330 core

layout (location = 0) out vec4 f_color;

in vec4 v_color;

uniform sampler2D u_depth;
uniform vec2 u_viewport_dimensions;

void main() {
    if (gl_FragCoord.z <= texture(u_depth, gl_FragCoord.xy / u_viewport_dimensions).r)
    {
        discard;
    }
    f_color = v_color;
}