#version 330 core

in vec2 v_tex_coord;
uniform sampler2D u_texture;
out vec4 o_frag_color;

void main()
{
    vec3 color = 1.0 - texture(u_texture, v_tex_coord).rgb;
    o_frag_color = vec4(color.rgb, 1.0f);
}
