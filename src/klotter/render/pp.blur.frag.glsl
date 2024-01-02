#version 330 core

#define SAMPLES {{sample_count}}

in vec2 v_tex_coord;
uniform sampler2D u_texture;
{{#is_horizontal}}
    uniform vec2 u_resolution;
{{/is_horizontal}}
uniform float u_factor;
uniform float u_blur_size;
out vec4 o_frag_color;


void main()
{
    {{#is_horizontal}}
        float inv_aspect = u_resolution.y / u_resolution.x;
    {{/is_horizontal}}
    
    vec3 blur = vec3(0,0,0);
    for(float index=0;index<SAMPLES;index++){
        float b = (index/(SAMPLES - 1) - 0.5) * u_blur_size * u_factor;
        
        {{#is_horizontal}}
            // horizontal blur
            vec2 offset = vec2(b * inv_aspect, 0);
        {{/is_horizontal}}
        
        {{#is_vertical}}
            // vertical blur
            vec2 offset = vec2(0, b);
        {{/is_vertical}}

        vec2 uv = v_tex_coord + offset;
        blur += texture(u_texture, uv).rgb;
    }
    o_frag_color = vec4(blur / SAMPLES, 1.0f);
}
