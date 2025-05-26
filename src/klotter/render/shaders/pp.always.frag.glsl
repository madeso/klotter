#version 330 core

in vec2 v_tex_coord;
uniform sampler2D u_texture;
out vec4 o_frag_color;

void main()
{
    vec4 color = texture(u_texture, v_tex_coord);
    o_frag_color = color;
}
