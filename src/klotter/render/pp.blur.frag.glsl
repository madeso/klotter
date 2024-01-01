#version 330 core

in vec2 v_tex_coord;
uniform sampler2D u_texture;
uniform float u_factor;
uniform float u_blur_size;
out vec4 o_frag_color;

void main()
{
    vec3 blur = vec3(0,0,0);
    for(float index=0;index<10;index++){
        // vertical blur
        vec2 uv = v_tex_coord + vec2(0, (index/9 - 0.5) * u_blur_size * u_factor);
        blur += texture(u_texture, uv).rgb;
    }
    o_frag_color = vec4(blur / 10.0f, 1.0f);
}
