#pragma once

#include "imgui.h"

namespace klotter
{

struct SCurve
{
	float s;
	float t;
};

/// x=[0,1] 0=left, 1=right, y=[0,1] 0=bottom, 1=top
SCurve s_curve_from_input(float x, float y);

// this takes floats instead of the curve since that means it could more easily be copied to glsl
float calculate_s_curve(float x, float s, float t);


bool imgui_s_curve_editor(const char* title, SCurve* sc, ImVec2* drag);

}