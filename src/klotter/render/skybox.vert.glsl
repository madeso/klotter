#version 330 core

///////////////////////////////////////////////////////////////////////////////
// attributes
in vec3 a_position;


///////////////////////////////////////////////////////////////////////////////
// uniforms
uniform mat4 u_projection;
uniform mat4 u_view;


///////////////////////////////////////////////////////////////////////////////
// varying
out vec3 v_tex_normal;


///////////////////////////////////////////////////////////////////////////////
// code
void main()
{
    gl_Position = (u_projection * u_view * vec4(a_position.xyz, 1.0)).xyww;
    v_tex_normal = a_position.xyz;
}