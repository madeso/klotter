#version 330 core

///////////////////////////////////////////////////////////////////////////////
// attributes
in vec3 a_position;
{{#use_lights}}
in vec3 a_normal;
{{/use_lights}}
in vec3 a_color;
in vec2 a_tex_coord;


///////////////////////////////////////////////////////////////////////////////
// uniforms
{{uniform_buffer_source}}

{{#use_instancing}}
in mat4 u_model; // hacky way to define a attribute :/
{{/use_instancing}}
{{^use_instancing}}
uniform mat4 u_model;
{{/use_instancing}}


///////////////////////////////////////////////////////////////////////////////
// varying
{{#use_lights}}
out vec3 v_worldspace;
out vec3 v_normal;
{{/use_lights}}
out vec3 v_color;
out vec2 v_tex_coord;


///////////////////////////////////////////////////////////////////////////////
// code
void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_position.xyz, 1.0);

{{#use_lights}}
    v_worldspace = vec3(u_model * vec4(a_position.xyz, 1.0));
    v_normal = mat3(transpose(inverse(u_model))) * a_normal; // move to cpu
{{/use_lights}}
    v_color = a_color;
    v_tex_coord = a_tex_coord;
}
