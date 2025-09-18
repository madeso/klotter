#version 330 core

///////////////////////////////////////////////////////////////////////////////
// attributes
in vec3 a_position;
{{#use_lights}}
in vec3 a_normal;
{{/use_lights}}
{{^only_depth}}
in vec3 a_color;
in vec2 a_tex_coord;
{{/only_depth}}

///////////////////////////////////////////////////////////////////////////////
// uniforms
{{uniform_buffer_source}}

{{#use_instancing}}
in mat4 u_world_from_local; // hacky way to define a attribute :/
{{/use_instancing}}
{{^use_instancing}}
uniform mat4 u_world_from_local;
{{/use_instancing}}


///////////////////////////////////////////////////////////////////////////////
// varying
{{#use_lights}}
out vec3 v_worldspace;
out vec3 v_normal;
{{/use_lights}}
{{^only_depth}}
out vec3 v_color;
out vec2 v_tex_coord;
{{/only_depth}}


///////////////////////////////////////////////////////////////////////////////
// code
void main()
{
    gl_Position = u_clip_from_view * u_view_from_world * u_world_from_local * vec4(a_position.xyz, 1.0);

{{#use_lights}}
    v_worldspace = vec3(u_world_from_local * vec4(a_position.xyz, 1.0));
    v_normal = mat3(transpose(inverse(u_world_from_local))) * a_normal; // move to cpu
{{/use_lights}}
{{^only_depth}}
    v_color = a_color;
    v_tex_coord = a_tex_coord;
{{/only_depth}}
}
