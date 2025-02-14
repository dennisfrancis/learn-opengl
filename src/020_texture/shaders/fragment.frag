#version 330 core

in vec3 out_color; // interpolated color from vertex shader.
in vec2 out_tex_coord; // interpolated tex coordinates from vertex shader.

out vec4 frag_color;

uniform sampler2D our_texture;

void main()
{
    // frag_color = vec4(out_color, 1.0);
    frag_color = texture(our_texture, out_tex_coord);
}
