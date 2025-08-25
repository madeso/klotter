#pragma once

#include "klotter/render/world.h"

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/

// todo(Gustav): switch to a full screen triangle instead
/// Store the information for how to render a full screen quad.
struct FullScreenGeom
{
	std::shared_ptr<CompiledGeom> geom;
	CompiledShaderVertexAttributes layout;

	FullScreenGeom();
};

/**
 * @}
*/

}  //  namespace klotter
