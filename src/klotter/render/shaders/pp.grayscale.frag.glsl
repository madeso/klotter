#version 330 core

in vec2 v_tex_coord;
uniform sampler2D u_texture;
uniform float u_factor;
out vec4 o_frag_color;

void main()
{
    vec3 color = texture(u_texture, v_tex_coord).rgb;
    float avg = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    vec3 grayscale = vec3(avg, avg, avg);
    o_frag_color = vec4(mix(color, grayscale, u_factor), 1.0f);
}
