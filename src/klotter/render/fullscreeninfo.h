#pragma once

#include "klotter/render/world.h"

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/

// todo(Gustav): rename to something better
// todo(Gustav): switch to a full screen triangle instead
/// Store the information on how to render a full screen quad.
struct FullScreenInfo
{
	std::shared_ptr<CompiledGeom> full_screen_geom;
	CompiledShaderVertexAttributes full_screen_layout;

	FullScreenInfo();
};

/**
 * @}
*/

}  //  namespace klotter
