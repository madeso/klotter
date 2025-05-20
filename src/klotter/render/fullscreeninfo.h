#pragma once

#include "klotter/render/world.h"

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/


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
