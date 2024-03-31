#version 330 core

layout(location = 0) out vec4 f_color;
layout(location = 1) out vec4 f_depth;

in vec4 v_color;

uniform sampler2D u_depth;

void main()
{
    float depth = texture(u_depth, gl_FragCoord.xy).r;
    if(gl_FragCoord.z < depth)
    {
        f_depth = vec4(gl_FragCoord.z, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        f_depth = vec4(gl_FragCoord.z, 1.0f, 1.0f, 1.0f);
    }
    f_color = vec4(vec3(v_color), 1.0f);
}