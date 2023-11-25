#version 330 core

///////////////////////////////////////////////////////////////////////////////
// uniforms
uniform vec4 u_tint_color;
uniform sampler2D u_tex_diffuse;
{{#use_lights}}
uniform vec3 u_view_position;
uniform vec3 u_light_color;
uniform vec3 u_light_world;
{{/use_lights}}


///////////////////////////////////////////////////////////////////////////////
// varying
{{#use_lights}}
in vec3 v_worldspace;
in vec3 v_normal;
{{/use_lights}}
in vec3 v_color;
in vec2 v_tex_coord;


///////////////////////////////////////////////////////////////////////////////
// output
out vec4 o_frag_color;


///////////////////////////////////////////////////////////////////////////////
// code
void main()
{
{{#use_lights}}
    float ambient_strength = 0.1;
    float specular_strength = 0.1;
    int material_shininess = 64;

    vec3 normal = normalize(v_normal);
    vec3 light_direction = normalize(u_light_world - v_worldspace);
    vec3 view_direction = normalize(u_view_position - v_worldspace);
    vec3 reflect_direction = reflect(-light_direction, normal);

    vec3 ambient_color = ambient_strength * u_light_color;     
    vec3 diffuse_color = max(dot(normal, light_direction), 0.0) * u_light_color;
    vec3 specular_color = specular_strength * pow(max(dot(view_direction, reflect_direction), 0.0), material_shininess) * u_light_color; 

    vec3 light_color = ambient_color + diffuse_color + specular_color;
{{/use_lights}}

    vec4 object_color = texture(u_tex_diffuse, v_tex_coord) * u_tint_color.rgba * vec4(v_color.rgb, 1.0);

    o_frag_color =

{{#use_lights}}
        vec4(light_color.rgb * object_color.rgb, object_color.a);
{{/use_lights}}

{{^use_lights}}
        object_color;
{{/use_lights}}

}
