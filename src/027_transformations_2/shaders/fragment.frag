#version 330 core

in vec3 out_color; // interpolated color from vertex shader.
in vec2 out_tex_coord; // interpolated tex coordinates from vertex shader.

out vec4 frag_color;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    vec2 flipped_tex_coord = vec2(1.0 - out_tex_coord.x, out_tex_coord.y);
    // 20% of the second texture and 80% of the first.
    frag_color = mix(texture(texture0, out_tex_coord), texture(texture1, flipped_tex_coord), 0.2);
}
