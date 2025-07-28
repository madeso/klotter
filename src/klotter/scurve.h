#pragma once

#include "imgui.h"

namespace klotter
{

/** \addtogroup scurcve S-Curve
 * \brief A tweakable curve similar to easing functions.
 * Discovered by Yann van der Cruyssen/Morusque on [twitter](https://x.com/Morusque/status/1352569197499441155).
 * From [A Convenient Generalization of Schlick’s Bias and Gain Functions (pdf)](https://arxiv.org/pdf/2010.09714) by Jonathan T. Barron.
 *  @{
*/

/// Contains the parameters for an S-Curve.
struct SCurve
{
	float slope = 1.0f;	 ///< >=0
	float threshold = 0.75f;  ///< [0,1]

	constexpr SCurve() = default;

	constexpr SCurve(float s, float t)
		: slope(s)
		, threshold(t)
	{}

	constexpr static SCurve light_curve()
	{
		return {5.0f, 1.0f};
	}
};

/// Represents the GUI state for an S-curve.
struct SCurveGuiState
{
	ImVec2 drag = {0.5f, 0.5f};	 ///< the gui data

	constexpr SCurveGuiState() = default;
	SCurveGuiState(float x, float y);

	static SCurveGuiState light_curve();

	std::vector<ImVec2> point_cache;
};


/// Generate an S-Curve from user input.
/// Adapted from [Colugo's](https://twitter.com/ColugoMusic/status/1363071439679729665?s=20) S curve editor implemented in/on [desmos](https://www.desmos.com/calculator/ibek4vkdiw)
/// @param x [0,1] 0=left, 1=right
/// @param y [0,1] 0=bottom, 1=top
SCurve s_curve_from_input(float x, float y);

/// Calculates an S-Curve.
/// this takes floats instead of the curve since that means it could more easily be copied to glsl
/// @param x [0,1]
/// @param slope >=0
/// @param threshold [0,1]
float calculate_s_curve(float x, float slope, float threshold);

/// Configuration settings for displaying and interacting with an S-curve widget.
struct SCurveImguiSettings
{
	bool widget_border = false;	 ///< draw a border around the widget
	ImVec2 widget_size = ImVec2{100, 100};	///< size of the widget

	float drag_radius = 10.0f;
	ImU32 drag_color = IM_COL32(100, 0, 0, 255);

	float point_radius = 3.0f;
	ImU32 point_color = IM_COL32(0, 100, 0, 255);
	bool draw_points = false;

	ImGuiMouseButton_ button = ImGuiMouseButton_Left;
	
	ImU32 background_color = IM_COL32(50, 50, 50, 255);
	ImU32 line_color = IM_COL32(100, 100, 100, 255);

	std::size_t num_points = 21;  ///< number of points to draw, more points means smoother curve, but more expensive
};

enum class FlipX
{
	no, yes
};

bool imgui_s_curve_editor(const char* title, SCurve* scurve, SCurveGuiState* gui, FlipX flip_x, const SCurveImguiSettings& settings, bool force_init_curve);

/**
 * @}
*/

}  //  namespace klotter
