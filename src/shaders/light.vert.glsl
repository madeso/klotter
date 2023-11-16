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
