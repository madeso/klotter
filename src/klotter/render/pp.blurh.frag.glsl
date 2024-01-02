#version 330 core

in vec2 v_tex_coord;
uniform sampler2D u_texture;
uniform vec2 u_resolution;
uniform float u_factor;
uniform float u_blur_size;
out vec4 o_frag_color;

#define SAMPLES 10

void main()
{
    float inv_aspect = u_resolution.y / u_resolution.x;
    vec3 blur = vec3(0,0,0);
    for(float index=0;index<SAMPLES;index++){
        // horizontal blur
        vec2 uv = v_tex_coord + vec2((index/(SAMPLES - 1) - 0.5) * u_blur_size * inv_aspect * u_factor, 0);
        blur += texture(u_texture, uv).rgb;
    }
    o_frag_color = vec4(blur / SAMPLES, 1.0f);
}
