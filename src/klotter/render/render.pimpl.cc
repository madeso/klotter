#include "klotter/render/render.pimpl.h"

#include "klotter/log.h"

#include "klotter/render/fullscreeninfo.h"
#include "klotter/render/shader_resource.h"

namespace klotter
{


std::string string_from_gl_bytes(const GLubyte* bytes)
{
	return reinterpret_cast<const char*>(bytes);
}

CameraUniformBuffer make_camera_uniform_buffer_desc()
{
	CameraUniformBuffer camera_uniform_buffer;

	{
		UniformBufferCompiler compiler;
		compiler.add(&camera_uniform_buffer.projection_prop, UniformType::mat4, "u_projection");
		compiler.add(&camera_uniform_buffer.view_prop, UniformType::mat4, "u_view");
		compiler.compile("Camera", &camera_uniform_buffer.setup, 0);
	}

	camera_uniform_buffer.buffer = std::make_unique<UniformBuffer>(camera_uniform_buffer.setup);

	return camera_uniform_buffer;
}


RendererPimpl::RendererPimpl(const RenderSettings& set, const FullScreenInfo& fsi)
	: camera_uniform_buffer(make_camera_uniform_buffer_desc())
	, shaders(load_shaders(camera_uniform_buffer, set, fsi))
	, full_screen_geom(fsi.full_screen_geom)
{
	const auto vendor = string_from_gl_bytes(glGetString(GL_VENDOR));
	const auto renderer = string_from_gl_bytes(glGetString(GL_RENDERER));
	const auto version = string_from_gl_bytes(glGetString(GL_VERSION));
	const auto shading_language_version = string_from_gl_bytes(glGetString(GL_SHADING_LANGUAGE_VERSION));
	const auto extensions = string_from_gl_bytes(glGetStringi(GL_EXTENSIONS, 0));

	LOG_INFO("vendor %s, renderer %s", vendor.c_str(), renderer.c_str());
	LOG_INFO("version %s (glsl %s)", version.c_str(), shading_language_version.c_str());
	LOG_INFO("extensions %s", extensions.c_str());
}


}  //  namespace klotter
