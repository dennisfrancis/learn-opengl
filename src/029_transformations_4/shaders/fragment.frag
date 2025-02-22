#version 330 core

in vec2 out_tex_coord; // interpolated tex coordinates from vertex shader.

out vec4 frag_color;

uniform sampler2D texture0;
uniform vec4 bgcolor;

void main()
{
    vec4 color = texture(texture0, out_tex_coord);
    frag_color = color.w < 1.0f ? bgcolor : color; 
}
