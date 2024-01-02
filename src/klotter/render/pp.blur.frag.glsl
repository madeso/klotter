#version 330 core

#define SAMPLES {{sample_count}}

in vec2 v_tex_coord;
uniform sampler2D u_texture;
{{#is_horizontal}}
    uniform vec2 u_resolution;
{{/is_horizontal}}

{{#is_gauss}}
    #define PI 3.14159265359
    #define E 2.71828182846
uniform float u_std_dev;
{{/is_gauss}}

uniform float u_factor;
uniform float u_blur_size;
out vec4 o_frag_color;


void main()
{
    {{#is_horizontal}}
        float inv_aspect = u_resolution.y / u_resolution.x;
    {{/is_horizontal}}
    
    vec3 blur = vec3(0,0,0);
    {{#is_gauss}}
        float sum = 0;
    {{/is_gauss}}

    for(float index=0;index<SAMPLES;index++){
        float offset = (index/(SAMPLES - 1) - 0.5) * u_blur_size * u_factor;
        
        {{#is_horizontal}}
            // horizontal blur
            vec2 uv = v_tex_coord + vec2(offset * inv_aspect, 0);
        {{/is_horizontal}}
        
        {{#is_vertical}}
            // vertical blur
            vec2 uv = v_tex_coord + vec2(0, offset);
        {{/is_vertical}}

        {{#is_gauss}}
            float std_dev_squared = u_std_dev*u_std_dev;
            float gauss = (1 / sqrt(2*PI*std_dev_squared)) * pow(E, -((offset*offset)/(2*std_dev_squared)));
            sum += gauss;
            blur += texture(u_texture, uv).rgb * gauss;
        {{/is_gauss}} {{^is_gauss}}
            blur += texture(u_texture, uv).rgb;
        {{/is_gauss}}
    }

    {{#is_gauss}}
        o_frag_color = vec4(blur / sum, 1.0f);
    {{/is_gauss}} {{^is_gauss}}
        o_frag_color = vec4(blur / SAMPLES, 1.0f);
    {{/is_gauss}}
}
