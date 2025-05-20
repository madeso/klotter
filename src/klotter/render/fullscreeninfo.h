#pragma once
#include "world.h"


namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/


struct FullScreenInfo
{
	std::shared_ptr<CompiledGeom> full_screen_geom;
	CompiledShaderVertexAttributes full_scrren_layout;

	FullScreenInfo();
};

/**
 * @}
*/

}  //  namespace klotter
