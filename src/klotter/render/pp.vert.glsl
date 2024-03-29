#version 330 core

///////////////////////////////////////////////////////////////////////////////
// attributes
in vec2 a_position;
in vec2 a_tex_coord;


///////////////////////////////////////////////////////////////////////////////
// uniforms

///////////////////////////////////////////////////////////////////////////////
// varying
out vec2 v_tex_coord;


///////////////////////////////////////////////////////////////////////////////
// code
void main()
{
    gl_Position = vec4(a_position.xy, 0.0, 1.0);
    v_tex_coord = a_tex_coord;
}
