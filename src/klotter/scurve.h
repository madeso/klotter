#pragma once

#include "imgui.h"

namespace klotter
{

/** \addtogroup scurcve S Curve
 * \brief A tweakable curve similar to easing functions.
 * Discovered by Yann van der Cruyssen/Morusque on [twitter](https://x.com/Morusque/status/1352569197499441155).
 * From [A Convenient Generalization of Schlick’s Bias and Gain Functions (pdf)](https://arxiv.org/pdf/2010.09714) by Jonathan T. Barron.
 *  @{
*/

/// Contains the paramters for a s curve.
struct SCurve
{
	float slope; ///< >=0
	float threshold; ///< [0,1]
};

/// This contains both a \ref SCurve and the ui for editing, game only needs scurve, ui only needs edit.
/// Defaults to a "linear" curve.
/// @see \ref imgui_s_curve_editor
struct SCurveAndDrag
{
	SCurve curve = {1.0f, 0.75f};
	ImVec2 drag = {0.5f, 0.5f};
};


/// Generate a S curve from user input.
/// Adapted from [Colugo's](https://twitter.com/ColugoMusic/status/1363071439679729665?s=20) S curve editor implemented in/on [desmos](https://www.desmos.com/calculator/ibek4vkdiw)
/// @param x [0,1] 0=left, 1=right
/// @param y [0,1] 0=bottom, 1=top
SCurve s_curve_from_input(float x, float y);

/// Calculates a s curve.
/// this takes floats instead of the curve since that means it could more easily be copied to glsl
/// @param x [0,1]
/// @param slope >=0
/// @param threshold [0,1]
float calculate_s_curve(float x, float slope, float threshold);


bool imgui_s_curve_editor(const char* title, SCurveAndDrag* scd, bool flip_x);

/**
 * @}
*/

}  //  namespace klotter
