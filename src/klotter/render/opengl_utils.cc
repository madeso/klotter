#include "klotter/render/opengl_utils.h"

#include "klotter/assert.h"
#include "klotter/dependency_sdl.h"

namespace klotter
{

const char* opengl_error_to_string(GLenum error_code)
{
	switch (error_code)
	{
	case GL_INVALID_ENUM: return "INVALID_ENUM";
	case GL_INVALID_VALUE: return "INVALID_VALUE";
	case GL_INVALID_OPERATION: return "INVALID_OPERATION";
#ifdef GL_STACK_OVERFLOW
	case GL_STACK_OVERFLOW: return "STACK_OVERFLOW";
#endif
#ifdef GL_STACK_UNDERFLOW
	case GL_STACK_UNDERFLOW: return "STACK_UNDERFLOW";
#endif
	case GL_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
	case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OPERATION";
	default: return "UNKNOWN";
	}
}

namespace
{
	const char* source_to_string(GLenum source)
	{
		switch (source)
		{
		case GL_DEBUG_SOURCE_API_ARB: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: return "Window System";
		case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: return "Shader Compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: return "Third Party";
		case GL_DEBUG_SOURCE_APPLICATION_ARB: return "Application";
		case GL_DEBUG_SOURCE_OTHER_ARB: return "Other";
		default: return "Unknown";
		}
	}

	const char* type_to_string(GLenum type)
	{
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR_ARB: return "Error";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: return "Deprecated Behaviour";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: return "Undefined Behaviour";
		case GL_DEBUG_TYPE_PORTABILITY_ARB: return "Portability";
		case GL_DEBUG_TYPE_PERFORMANCE_ARB: return "Performance";
		case GL_DEBUG_TYPE_OTHER_ARB: return "Other";
		default: return "Unknown";
		}
	}

	const char* severity_to_string(GLenum severity)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH_ARB: return "high";
		case GL_DEBUG_SEVERITY_MEDIUM_ARB: return "medium";
		case GL_DEBUG_SEVERITY_LOW_ARB: return "low";
		default: return "unknown";
		}
	}

}  // namespace

void APIENTRY on_opengl_error(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei /*length*/,
	const GLchar* message,
	const GLvoid* /*userParam*/
)
{
	// ignore non-significant error/warning codes
	if (type == GL_DEBUG_TYPE_OTHER_ARB)
	{
		return;
	}

	// only display the first 10
	static int ErrorCount = 0;
	if (ErrorCount > 10)
	{
		return;
	}
	++ErrorCount;

	SDL_LogCritical(
		SDL_LOG_CATEGORY_ERROR,
		"---------------\n"
		"Debug message (%d): %s\n"
		"Source %s type: %s Severity: %s",
		id,
		message,
		source_to_string(source),
		type_to_string(type),
		severity_to_string(severity)
	);
	DIE("OpenGL error");
}

bool has_debug_label()
{
	return GLAD_GL_EXT_debug_label && glad_glLabelObjectEXT;
}

bool has_khr_debug()
{
	return GLAD_GL_KHR_debug;
}

void setup_opengl_debug()
{
	if (has_khr_debug())
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Enabling OpenGL debug output (KHR_debug)");
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
		glDebugMessageCallback(on_opengl_error, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	else
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KHR_debug not available, OpenGL debug output not enabled");
	}
}

GLenum Cint_to_glenum(int i)
{
	return static_cast<GLenum>(i);
}

GLuint Cint_to_gluint(int i)
{
	return static_cast<GLuint>(i);
}

GLsizeiptr Csizet_to_glsizeiptr(std::size_t t)
{
	return static_cast<GLsizeiptr>(t);
}

GLsizei Csizet_to_glsizei(std::size_t t)
{
	return static_cast<GLsizei>(t);
}

u32 create_buffer()
{
	u32 vbo;
	glGenBuffers(1, &vbo);
	return vbo;
}

void destroy_buffer(u32 vbo)
{
	glDeleteBuffers(1, &vbo);
}

u32 create_vertex_array()
{
	u32 vao;
	glGenVertexArrays(1, &vao);
	return vao;
}

void destroy_vertex_array(u32 vao)
{
	glDeleteVertexArrays(1, &vao);
}

void set_gl_viewport(const glm::ivec2& sz)
{
	glViewport(0, 0, sz.x, sz.y);
}

glm::mat4 get_mesh_rotation_matrix(const glm::vec3& rotation)
{
	return glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
}

void set_gl_debug_label(GLenum type, GLuint object, const char* label)
{
    if (has_debug_label())
    {
        glad_glLabelObjectEXT(type, object, -1, label);
    }
}
}  //  namespace klotter
