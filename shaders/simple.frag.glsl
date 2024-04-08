#version 330 core

layout(location = 0) out vec4 f_color;
layout(location = 1) out vec3 f_depth;

in vec3 v_color;

void main() {
    f_depth = vec3(gl_FragCoord.z, 0.0f, 0.0f); 
    f_color = vec4(v_color, 1.0f);
}