#version 330 core

///////////////////////////////////////////////////////////////////////////////
// uniforms
uniform vec4 u_tint_color; // diffuse + alpha
uniform sampler2D u_tex_diffuse;

{{#use_lights}}
uniform sampler2D u_tex_specular;
uniform sampler2D u_tex_emissive;

uniform float u_emissive_factor;
uniform vec3 u_ambient_tint;
uniform vec3 u_specular_color;
uniform float u_shininess;

uniform vec3 u_view_position;
// uniform vec3 u_light_color;
uniform vec3 u_light_ambient_color;
uniform vec3 u_light_diffuse_color;
uniform vec3 u_light_specular_color;

uniform vec3 u_light_world;
{{/use_lights}}


///////////////////////////////////////////////////////////////////////////////
// varying
in vec3 v_color;
in vec2 v_tex_coord;

{{#use_lights}}
in vec3 v_worldspace;
in vec3 v_normal;
{{/use_lights}}


///////////////////////////////////////////////////////////////////////////////
// output
out vec4 o_frag_color;


///////////////////////////////////////////////////////////////////////////////
// code
void main()
{
{{#use_lights}}
    vec3 normal = normalize(v_normal);
    vec3 light_direction = normalize(u_light_world - v_worldspace);
    vec3 view_direction = normalize(u_view_position - v_worldspace);
    vec3 reflect_direction = reflect(-light_direction, normal);
    vec4 tex = texture(u_tex_diffuse, v_tex_coord);
    vec3 spec_t = texture(u_tex_specular, v_tex_coord).rgb;
    vec3 emi_t = texture(u_tex_emissive, v_tex_coord).rgb;
    vec3 base_color = tex.rgb * v_color.rgb;
    float alpha = tex.a * u_tint_color.a;

    // ambient color
    vec3 ambient_color = u_ambient_tint * base_color * u_light_ambient_color;

    // diffuse color
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse_color = diff * (u_tint_color.rgb * base_color * u_light_diffuse_color);

    // specular color
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), u_shininess);
    vec3 specular_color = spec * (u_specular_color * spec_t * u_light_specular_color);

    // emissive color
    vec3 emissive_color = u_emissive_factor * emi_t;

    vec3 light_color = ambient_color + diffuse_color + specular_color + emissive_color;

    o_frag_color = vec4(light_color.rgb, alpha);
{{/use_lights}}
{{^use_lights}}
    vec4 object_color = texture(u_tex_diffuse, v_tex_coord) * u_tint_color.rgba * vec4(v_color.rgb, 1.0);
    o_frag_color = object_color;
{{/use_lights}}

}
