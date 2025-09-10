#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertex_color;

uniform mat4 tr_mat;

out vec3 out_color;

void main()
{
    gl_Position = tr_mat * vec4(pos, 1.0);
    out_color = vertex_color;
}
