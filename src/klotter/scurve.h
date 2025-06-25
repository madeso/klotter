#pragma once

#include "imgui.h"

namespace klotter
{

/** \addtogroup scurcve S Curve
 * Discovered in https://x.com/Morusque/status/1352569197499441155.
 * Expanded in [A Convenient Generalization of Schlick’s Bias and Gain Functions (pdf)](https://arxiv.org/pdf/2010.09714) by Jonathan T. Barron.
 *  @{
*/

/// Contains the paramters for a s curve.
struct SCurve
{
	float s;
	float t;
};

/// This contains both a \ref SCurve and the ui for editing, game only needs scurve, ui only needs edit.
/// Defaults to a "linear" curve.
/// @see \ref imgui_s_curve_editor
struct SCurveAndDrag
{
	SCurve curve = {1.0f, 0.75f};
	ImVec2 drag = {0.5f, 0.5f};
};

/// @param x [0,1] 0=left, 1=right
/// @param y [0,1] 0=bottom, 1=top
SCurve s_curve_from_input(float x, float y);

/// Calculates a s curve.
/// this takes floats instead of the curve since that means it could more easily be copied to glsl
float calculate_s_curve(float x, float s, float t);


bool imgui_s_curve_editor(const char* title, SCurveAndDrag* scd, bool flipx);

/**
 * @}
*/

}  //  namespace klotter
