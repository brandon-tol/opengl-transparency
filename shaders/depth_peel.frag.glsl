#version 330 core

layout (location = 0) out vec4 f_color;
layout (location = 1) out vec4 f_depth;

in vec3 v_color;

uniform sampler2D u_depth;

void main() {
    float depth = texture(u_depth, gl_FragCoord.xy).r;
    float curr_depth = gl_FragCoord.z;


    if (curr_depth - depth <= 0.901f) // essentially gl_FragCoord.z > depth with a tolerance of 0.0001
        discard;
    else
    {
        f_color = vec4(v_color, 1.0f);
        f_depth = vec4(curr_depth, 0.0f, 0.0f, 1.0f);
    }
    // f_color = vec4(depth, depth, depth, 1.0);
}