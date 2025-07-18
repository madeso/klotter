#pragma once

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/

// todo(Gustav): figure out what values should force the renderer to restart.
/// Startup settings for the renderer.
struct RenderSettings
{
	int number_of_directional_lights = 5;
	int number_of_point_lights = 5;
	int number_of_frustum_lights = 5;

	int msaa = 4;

	/// The gamma correction value.
	/// The renderer doesn't need to restart when this value has changed.
	float gamma = 2.2f;
};

/**
 * @}
*/

}  //  namespace klotter
