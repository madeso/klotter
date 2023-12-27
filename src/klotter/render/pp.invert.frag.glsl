#version 330 core

in vec2 v_tex_coord;
uniform sampler2D u_texture;
uniform float u_factor;
out vec4 o_frag_color;

void main()
{
    vec3 color = texture(u_texture, v_tex_coord).rgb;
    vec3 inverted = 1 - color;
    o_frag_color = vec4(mix(color, inverted, u_factor), 1.0f);
}
