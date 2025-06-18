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

enum class DebugLabelFor
{
    Buffer,
    Shader,
    Program,
    VertexArray,
    Query,
    ProgramPipeline,
    Sampler,
    Texture,
    RenderBuffer,
    FrameBuffer
};

void set_gl_debug_label(DebugLabelFor type, GLuint object, const std::string& label);
void set_gl_debug_label(DebugLabelFor type, GLuint object, std::string_view label);

#define DEBUG_LABEL_ARG_MANY const std::string& debug_label,

#define SET_DEBUG_LABEL(ID, FOR) set_gl_debug_label(FOR, ID, debug_label)
#define SET_DEBUG_LABEL_NAMED(ID, FOR, NAME) set_gl_debug_label(FOR, ID, NAME)
#define USE_DEBUG_LABEL(name) name,

// debug labels
#define USE_PROGRAM_LABEL(name) USE_DEBUG_LABEL(name)

struct ScopedDebugGroup
{
	explicit ScopedDebugGroup(const std::string& message, unsigned int id=0);
	explicit ScopedDebugGroup(std::string_view message, unsigned int id=0);
    ~ScopedDebugGroup();

    ScopedDebugGroup(ScopedDebugGroup&&) = delete;
	ScopedDebugGroup(const ScopedDebugGroup&) = delete;
    void operator=(ScopedDebugGroup&&) = delete;
	void operator=(const ScopedDebugGroup&) = delete;
};

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define SCOPED_DEBUG_GROUP(TEXT) [[maybe_unused]] const ScopedDebugGroup CONCAT(sc, __LINE__){TEXT}

}  //  namespace klotter
