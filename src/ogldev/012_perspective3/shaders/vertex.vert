#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertex_color;

uniform mat4 trans_tot;

out vec3 out_color;

void main()
{
    gl_Position = trans_tot * vec4(pos, 1.0);
    out_color = vertex_color;
}
