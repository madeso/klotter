#include "klotter/render/opengl_utils.h"

#include "klotter/assert.h"
#include "klotter/dependency_sdl.h"
#include "klotter/str.h"

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
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
		case GL_DEBUG_SOURCE_APPLICATION: return "Application";
		case GL_DEBUG_SOURCE_OTHER: return "Other";
		default: return "Unknown";
		}
	}

	const char* type_to_string(GLenum type)
	{
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "Error";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
		case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
		case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
		case GL_DEBUG_TYPE_OTHER: return "Other";
		default: return "Unknown";
		}
	}

	const char* severity_to_string(GLenum severity)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH: return "High";
		case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
		case GL_DEBUG_SEVERITY_LOW: return "Low";
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
		default: return "Unknown";
		}
	}

}  // namespace

void APIENTRY on_opengl_debug_output(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei /*length*/,
	const GLchar* message,
	const GLvoid* /*userParam*/
)
{
	switch (type)
	{
	// this is from ScopedDebugGroup
	case GL_DEBUG_TYPE_PUSH_GROUP:
	case GL_DEBUG_TYPE_POP_GROUP:
		return;
	default:
		break;
	}

	// ignore non-significant error/warning codes
	const auto is_important = type != GL_DEBUG_TYPE_OTHER;
	const bool is_low = severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION;

	if (is_important == false && is_low)
	{
		/*
		Tries to hide the following notification:
		 Buffer object 40 (bound to GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (0),
		 	GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (1), GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (2),
			and GL_ARRAY_BUFFER_ARB, usage hint is GL_STREAM_DRAW) will use VIDEO memory as the source
			for buffer object operations.
		 Buffer object 41 (bound to GL_ELEMENT_ARRAY_BUFFER_ARB, usage hint is
		 	GL_STREAM_DRAW) will use VIDEO memory as the source for buffer object operations.
		 */
		return;
	}

	// only display the first 10 notifications
	static int error_count = 0;
	if (is_important == false)
	{
		if (error_count > 10)
		{
			return;
		}
		++error_count;
	}

	const std::string to_out = Str() <<
		"OpenGL #" << id << " ["
			"src: "<< source_to_string(source) << " | "
			"type: " << type_to_string(type) << " | "
			"sev: " << severity_to_string(severity) << "] "
			": " << message
	;

	if (is_low)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", to_out.c_str());
	}
	else
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", to_out.c_str());
	}

	if (is_important)
	{
		DIE("OpenGL error");
	}
}

bool has_khr_debug()
{
	return GLAD_GL_KHR_debug != 0;
}

void setup_opengl_debug()
{
	if (has_khr_debug())
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Enabling OpenGL debug output (KHR_debug)");
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
		glDebugMessageCallback(on_opengl_debug_output, nullptr);
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
	u32 vbo = 0;
	glGenBuffers(1, &vbo);
	return vbo;
}

void destroy_buffer(u32 vbo)
{
	glDeleteBuffers(1, &vbo);
}

u32 create_vertex_array()
{
	u32 vao = 0;
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

GLenum glenum_from_object_type(DebugLabelFor type)
{
    switch (type)
    {
	case DebugLabelFor::Buffer: return GL_BUFFER;
	case DebugLabelFor::Shader: return GL_SHADER;
	case DebugLabelFor::Program: return GL_PROGRAM;
	case DebugLabelFor::VertexArray: return GL_VERTEX_ARRAY;
	case DebugLabelFor::Query: return GL_QUERY;
	case DebugLabelFor::ProgramPipeline: return GL_PROGRAM_PIPELINE;
	case DebugLabelFor::Sampler: return GL_SAMPLER;
	case DebugLabelFor::Texture: return GL_TEXTURE;
	case DebugLabelFor::RenderBuffer: return GL_RENDERBUFFER;
	case DebugLabelFor::FrameBuffer: return GL_FRAMEBUFFER;
    default:                          return GL_NONE;
    }
}

void set_gl_debug_label_with_size(DebugLabelFor type, GLuint object, std::size_t size, const char* label)
{
	if (! has_khr_debug())
	{
		return;
	}

    const auto gl_type = glenum_from_object_type(type);
	if (gl_type == GL_NONE)
	{
		return;
	}
	
	glObjectLabel(gl_type, object, Csizet_to_glsizei(size), label);
}

void set_gl_debug_label(DebugLabelFor type, GLuint object, const std::string& label)
{
	set_gl_debug_label_with_size(type, object, label.size(), label.data());
}

void set_gl_debug_label(DebugLabelFor type, GLuint object, std::string_view label)
{
	set_gl_debug_label_with_size(type, object, label.size(), label.data());
}

void push_debug_group(unsigned int id, std::size_t size, const char* label)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, id, Csizet_to_glsizei(size), label);
}

ScopedDebugGroup::ScopedDebugGroup(const std::string& message, unsigned int id)
{
    push_debug_group(id, message.size(), message.data());
}

ScopedDebugGroup::ScopedDebugGroup(std::string_view message, unsigned int id)
{
    push_debug_group(id, message.size(), message.data());
}

ScopedDebugGroup::~ScopedDebugGroup()
{
    glPopDebugGroup();
}

}  //  namespace klotter
