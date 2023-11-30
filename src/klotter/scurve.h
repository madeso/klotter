#pragma once

#include "imgui.h"

namespace klotter
{

struct SCurve
{
	float s;
	float t;
};

/// Both s curve and ui for editing, game only needs scurve, ui only needs edit.
/// Defaults to a "linear" curve.
struct SCurveAndDrag
{
	SCurve curve = {1.0f, 0.75f};
	ImVec2 drag = {0.5f, 0.5f};
};

/// x=[0,1] 0=left, 1=right, y=[0,1] 0=bottom, 1=top
SCurve s_curve_from_input(float x, float y);

// this takes floats instead of the curve since that means it could more easily be copied to glsl
float calculate_s_curve(float x, float s, float t);


bool imgui_s_curve_editor(const char* title, SCurveAndDrag* scd, bool flipx);

}  //  namespace klotter
