#pragma once

namespace klotter
{


void setup_opengl_debug();


GLenum Cint_to_glenum(int i);
GLuint Cint_to_gluint(int i);
GLsizeiptr Csizet_to_glsizeiptr(std::size_t t);
GLsizei Csizet_to_glsizei(std::size_t t);

}
