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


bool simple_gamma_slider(const char* label, float* gamma, float curve, float min_gamma, float max_gamma)
{
	if (curve < 0.0f)
	{
		return ImGui::SliderFloat(label, gamma, min_gamma, max_gamma);
	}

	// todo(Gustav): is this the correct way? it doesn't feel exactly right but perhaps that's just dear imgui
	const auto gamma_range = max_gamma - min_gamma;
	const auto t = (*gamma - min_gamma) / (gamma_range);

	auto slider_value = std::pow(t, 1.0f / curve);
	if (ImGui::SliderFloat(label, &slider_value, 0.0f, 1.0f) == false)
	{
		return false;
	}

	const auto perceptual = std::pow(slider_value, curve);
	*gamma = min_gamma + perceptual * gamma_range;
	return true;
}

}