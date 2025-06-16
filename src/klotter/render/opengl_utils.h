#pragma once

namespace klotter
{


void setup_opengl_debug();


GLenum Cint_to_glenum(int i);
GLuint Cint_to_gluint(int i);
GLsizeiptr Csizet_to_glsizeiptr(std::size_t t);
GLsizei Csizet_to_glsizei(std::size_t t);

u32 create_buffer();
void destroy_buffer(u32 vbo);

u32 create_vertex_array();
void destroy_vertex_array(u32 vao);

void set_gl_viewport(const glm::ivec2& sz);

// todo(Gustav): move to a better place
glm::mat4 get_mesh_rotation_matrix(const glm::vec3& rotation);

// Debug label utilities (GL_EXT_debug_label)
void set_gl_debug_label(GLenum type, GLuint object, const char* label);

}  //  namespace klotter
