#version 330 core

///////////////////////////////////////////////////////////////////////////////
// uniforms

struct Material
{
    vec4 diffuse_tint; // diffuse + alpha
    sampler2D diffuse_tex;

    {{#use_lights}}
    sampler2D specular_tex;
    sampler2D emissive_tex;
    float emissive_factor;
    vec3 ambient_tint;
    vec3 specular_tint;
    float shininess;
    {{/use_lights}}
};

uniform Material u_material;

{{#use_lights}}
uniform vec3 u_view_position;

uniform vec3 u_ambient_light;

struct PointLight
{
    vec3 diffuse;
    vec3 specular;
    vec4 attenuation; // min max s t

    vec3 world_pos;
};

uniform PointLight u_point_lights[{{number_of_pointlights}}];
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
// s curve
{{#use_lights}}

float calculate_s_curve(float x, float s, float t)
{
	float mE = 0.00001f;	// machine epsilon
	return x < t ? t * x / (x + s * (t - x) + mE)
				 : ((1 - t) * (x - 1)) / (1 - x - s * (t - x) + mE) + 1;
}

float calculate_attenuation(vec4 att, float distance)
{
    float min_dist = att.x;
    float max_dist = att.y;
    float scale = (distance - min_dist) / (max_dist - min_dist); // 0 at min, 1 at max
    float attenuation = 1.0 - calculate_s_curve(clamp(scale, 0, 1), att.z, att.w);
    return attenuation; // 1 at min, 0 at max
}

vec3 calculate_point_light(
    PointLight pl, vec3 normal, vec3 view_direction, vec3 spec_t, vec3 base_color)
{
    vec3 light_direction = normalize(pl.world_pos - v_worldspace);
    vec3 reflect_direction = reflect(-light_direction, normal);

    // diffuse color
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse_color = diff * (u_material.diffuse_tint.rgb * base_color * pl.diffuse);

    // specular color
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), u_material.shininess);
    vec3 specular_color = spec * (u_material.specular_tint * spec_t * pl.specular);

    // attenuation
    float distance = length(pl.world_pos - v_worldspace);
    float attenuation = calculate_attenuation(pl.attenuation, distance);

    return (diffuse_color + specular_color) * attenuation;
}
{{/use_lights}}

///////////////////////////////////////////////////////////////////////////////
// code
void main()
{
{{#use_lights}}
    vec3 normal = normalize(v_normal);
    vec3 view_direction = normalize(u_view_position - v_worldspace);
    vec4 tex = texture(u_material.diffuse_tex, v_tex_coord);
    vec3 spec_t = texture(u_material.specular_tex, v_tex_coord).rgb;
    vec3 emi_t = texture(u_material.emissive_tex, v_tex_coord).rgb;
    vec3 base_color = tex.rgb * v_color.rgb;
    float alpha = tex.a * u_material.diffuse_tint.a;

    // ambient color
    vec3 ambient_color = u_material.ambient_tint * base_color * u_ambient_light;

    // emissive color
    vec3 emissive_color = u_material.emissive_factor * emi_t;

    vec3 light_color = ambient_color + emissive_color;

    // point lights
    for(int i=0; i<{{number_of_pointlights}}; i+=1)
    {
        light_color += calculate_point_light(u_point_lights[i], normal, view_direction, spec_t, base_color);
    }

    o_frag_color = vec4(light_color.rgb, alpha);
{{/use_lights}}
{{^use_lights}}
    vec4 object_color = texture(u_material.diffuse_tex, v_tex_coord)
        * u_material.diffuse_tint.rgba * vec4(v_color.rgb, 1.0);
    o_frag_color = object_color;
{{/use_lights}}

}
