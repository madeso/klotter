#include "klotter/render/shader.source.h"

using namespace std::literals::string_view_literals;

// todo(Gustav): move glsl to embedded resource files

namespace klotter
{


ShaderSource basic_shader_source()
{
	return {
		{{VertexType::position3, "a_position"},
		 {VertexType::color3, "a_color"},
		 {VertexType::texture2, "a_tex_coord"}},
		R"glsl(
    #version 330 core

    in vec3 a_position;
    in vec3 a_color;
    in vec2 a_tex_coord;

    uniform mat4 u_projection;
    uniform mat4 u_view;
    uniform mat4 u_model;

    out vec3 v_color;
    out vec2 v_tex_coord;

    void main()
    {
        gl_Position = u_projection * u_view * u_model * vec4(a_position.xyz, 1.0);
        v_color = a_color;
        v_tex_coord = a_tex_coord;
    }
)glsl"sv,
		R"glsl(
    #version 330 core

    uniform vec4 u_tint_color;
    uniform sampler2D u_tex_diffuse;

    in vec3 v_color;
    in vec2 v_tex_coord;

    out vec4 o_frag_color;

    void main()
    {
        o_frag_color = texture(u_tex_diffuse, v_tex_coord) * u_tint_color.rgba * vec4(v_color.rgb, 1.0);
    }
)glsl"sv};
}

ShaderSource light_shader_source()
{
	return {
		{{VertexType::position3, "a_position"},
		 {VertexType::normal3, "a_normal"},
		 {VertexType::color3, "a_color"},
		 {VertexType::texture2, "a_tex_coord"}},
		R"glsl(
    #version 330 core

    in vec3 a_position;
    in vec3 a_normal;
    in vec3 a_color;
    in vec2 a_tex_coord;

    uniform mat4 u_projection;
    uniform mat4 u_view;
    uniform mat4 u_model;

    out vec3 v_worldspace;
    out vec3 v_normal;
    out vec3 v_color;
    out vec2 v_tex_coord;

    void main()
    {
        gl_Position = u_projection * u_view * u_model * vec4(a_position.xyz, 1.0);
        v_worldspace = vec3(u_model * vec4(a_position.xyz, 1.0));
        v_normal = mat3(transpose(inverse(u_model))) * a_normal; // move to cpu
        v_color = a_color;
        v_tex_coord = a_tex_coord;
    }
)glsl"sv,
		R"glsl(
    #version 330 core

    uniform vec4 u_tint_color;
    uniform sampler2D u_tex_diffuse;
    uniform vec3 u_view_position;
    uniform vec3 u_light_color;
    uniform vec3 u_light_world;

    in vec3 v_worldspace;
    in vec3 v_normal;
    in vec3 v_color;
    in vec2 v_tex_coord;

    out vec4 o_frag_color;

    void main()
    {
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

        vec4 object_color = texture(u_tex_diffuse, v_tex_coord) * u_tint_color.rgba * vec4(v_color.rgb, 1.0);

        o_frag_color = vec4(light_color.rgb * object_color.rgb, object_color.a);
    }
)glsl"sv};
}

}  //  namespace klotter
