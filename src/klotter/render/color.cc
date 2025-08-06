#include "klotter/render/color.h"

#include "imgui.h"

#include <cmath>

namespace klotter
{

float linear_from_srgb(float value, float gamma)
{
	// converts from gamma space to linear space

	// todo(Gustav): is this srgb or a basic gamma2 transformation? what's the difference? verify all code usage!
	return std::pow(value, gamma);
}




glm::vec3 linear_from_srgb(const Color& value, float gamma)
{
	return
	{
		linear_from_srgb(value.r, gamma),
		linear_from_srgb(value.g, gamma),
		linear_from_srgb(value.b, gamma)
	};
}


}