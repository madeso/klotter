#pragma once

#include "klotter/render/linebatch.h"
#include "klotter/render/opengl_states.h"
#include "klotter/render/shader_resource.h"
#include "klotter/render/world.h"

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/


struct RendererPimpl
{
	CameraUniformBuffer camera_uniform_buffer;
	ShaderResource shaders;
	OpenglStates states;
	DebugDrawer debug;
	std::shared_ptr<CompiledGeom> full_screen_geom;

	RendererPimpl(const RenderSettings& set, const FullScreenInfo& fsi);
};


/**
 * @}
*/

}  //  namespace klotter
