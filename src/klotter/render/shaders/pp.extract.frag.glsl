#version 330 core

uniform sampler2D u_texture;
uniform float u_cutoff;

in vec2 v_tex_coord;

out vec4 o_frag_color;

void main()
{
    vec4 sampled = texture(u_texture, v_tex_coord);
    float alpha = sampled.a;
    vec3 color = sampled.rgb;

    // calculates "relative luminance" from linear color space: https://en.wikipedia.org/wiki/Relative_luminance
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    if(brightness <= u_cutoff)
    {
        color = vec3(0, 0, 0);
    }

    o_frag_color = vec4(color, alpha);
}
