#pragma once

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/

/// Startup settings for the renderer.
struct RenderSettings
{
	int number_of_directional_lights = 5;
	int number_of_point_lights = 5;
	int number_of_frustum_lights = 5;

	int msaa = 4;
};

/**
 * @}
*/

}  //  namespace klotter
