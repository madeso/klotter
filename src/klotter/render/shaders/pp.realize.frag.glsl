#version 330 core

uniform sampler2D u_texture;
uniform float u_gamma;

in vec2 v_tex_coord;

out vec4 o_frag_color;

void main()
{
    vec4 color = texture(u_texture, v_tex_coord);
    vec3 corrected = pow(color.rgb, vec3(1.0f/u_gamma));
    o_frag_color = vec4(corrected, color.a);
}
