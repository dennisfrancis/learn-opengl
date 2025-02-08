#version 330 core

out vec4 frag_color;
uniform vec4 my_color;

void main()
{
    frag_color = my_color;
}
