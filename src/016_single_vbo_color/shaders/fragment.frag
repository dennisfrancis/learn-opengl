#version 330 core

in vec3 out_color; // interpolated color from vertex shader.
out vec4 frag_color;

void main()
{
    frag_color = vec4(out_color, 1.0);
}
