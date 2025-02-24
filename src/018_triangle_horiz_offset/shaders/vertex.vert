#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertex_color;

uniform float x_offset;
out vec3 out_color;

void main()
{
    gl_Position = vec4(pos.x + x_offset, pos.y, pos.z, 1.0);
    out_color = vertex_color;
}
