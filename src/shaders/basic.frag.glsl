#version 330 core

uniform vec4 u_tint_color;
uniform sampler2D u_tex_diffuse;

in vec3 v_color;
in vec2 v_tex_coord;

out vec4 o_frag_color;

void main()
{
    o_frag_color = texture(u_tex_diffuse, v_tex_coord) * u_tint_color.rgba * vec4(v_color.rgb, 1.0);
}
