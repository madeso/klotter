#include "klotter/scurve.h"

#include "klotter/assert.h"
#include <cmath>

#include "imgui.h"

namespace klotter
{

float square(float x)
{
	return x * x;
}

SCurve s_curve_from_input(float mx, float my)
{
	// s curve
	// Colugo's curve editor: https://twitter.com/ColugoMusic/status/1363071439679729665?s=20
	// https://www.desmos.com/calculator/ibek4vkdiw
	// which is based off of Yann van der Cruyssen's sCurve formula: https://twitter.com/Morusque/status/1352569197499441155
	// which is based off of this paper: https://arxiv.org/pdf/2010.09714.pdf

	const auto p0 = mx;
	// gustav addition: switch p1 to match curve to xy
	const auto p1 = mx > 0.5f ? my : 1 - my;

	const auto o = [](float x)
	{
		return x < 0.5f ? -0.5f * (2.0f * x * (2.0f * x - 2.0f)) : 2.0f * (square(x - 0.5f)) + 0.5f;
	};
	const auto q0 = o(p0);
	const auto q1 = o(p1);

	const auto a = [](float x)
	{
		return std::pow(3.0f, x);
	};
	const auto s = a(10 * (q0 - 0.5f));
	const auto t = 0.5f + (0.5f * (1.0f - q1));
	return {s, t};
}

float calculate_s_curve(float x, float s, float t)
{
	constexpr float mE = 0.00001f;	// machine epsilon
	return x < t ? t * x / (x + s * (t - x) + mE) : ((1 - t) * (x - 1)) / (1 - x - s * (t - x) + mE) + 1;
}

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y};
}

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return {lhs.x - rhs.x, lhs.y - rhs.y};
}

bool imgui_s_curve_editor(const char* title, SCurveAndDrag* scd, bool flip_x)
{
	ImGui::Text("%s (%f %f)", title, static_cast<double>(scd->curve.s), static_cast<double>(scd->curve.t));
	if (ImGui::BeginChild(title, ImVec2{100, 100}, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)
		== false)
	{
		ImGui::EndChild();
		return false;
	}

	constexpr float radius = 10.0f;
	constexpr float dot_radius = 3.0f;
	constexpr auto button = ImGuiMouseButton_Left;
	constexpr std::size_t max_points = 20;
	constexpr auto dot_color = IM_COL32(0, 100, 0, 255);
	constexpr auto background_color = IM_COL32(50, 50, 50, 255);
	constexpr auto line_color = IM_COL32(100, 100, 100, 255);
	constexpr auto drag_color = IM_COL32(100, 0, 0, 255);
	constexpr auto draw_points = false;

	bool changed = false;

	const auto pos = ImGui::GetWindowPos();
	const auto size = ImGui::GetWindowSize();

	auto* draw = ImGui::GetWindowDrawList();
	draw->AddRectFilled(pos, pos + size, background_color);

	std::array<ImVec2, max_points + 1> points;
	for (std::size_t i = 0; i < max_points + 1; i += 1)
	{
		const float srcx = static_cast<float>(i) / static_cast<float>(max_points);
		const float x = flip_x ? 1 - srcx : srcx;
		const float y = calculate_s_curve(srcx, scd->curve.s, scd->curve.t);
		points[i] = ImVec2{x * size.x, (1 - y) * size.y} + pos;
	}
	draw->AddPolyline(points.data(), max_points + 1, line_color, ImDrawFlags_None, 1.0f);

	if (draw_points)
	{
		for (std::size_t i = 0; i < max_points + 1; i += 1)
		{
			draw->AddCircleFilled(points[i], dot_radius, dot_color);
		}
	}
	draw->AddCircleFilled(ImVec2{scd->drag.x * size.x, (1 - scd->drag.y) * size.y} + pos, radius, drag_color);

	const auto t01 = [&pos, &size](const ImVec2& mp)
	{
		const auto x = (mp.x - pos.x) / size.x;
		const auto y = 1 - ((mp.y - pos.y) / size.y);
		return ImVec2{x, y};
	};

	const auto within01 = [](const ImVec2& p)
	{
#define T(s) \
	do \
	{ \
		if (s < 0.0f) \
			return false; \
		else if (s > 1.0f) \
			return false; \
	} while (false)
		T(p.x);
		T(p.y);
		return true;
#undef T
	};

	if (ImGui::IsMouseDown(button) && within01(t01(ImGui::GetIO().MouseClickedPos[button])))
	{
		// mouse is down and started inside the component
		const auto mp = ImGui::GetMousePos();
		const auto n = t01(mp);
		scd->drag.x = std::min(1.0f, std::max(n.x, 0.0f));
		scd->drag.y = std::min(1.0f, std::max(n.y, 0.0f));
		scd->curve = s_curve_from_input(flip_x ? 1 - scd->drag.x : scd->drag.x, scd->drag.y);
		changed = true;
	}

	ImGui::EndChild();

	return changed;
}

#if 0
struct ShittyUnitTest1
{
	static constexpr bool feq(float lhs, float rhs)
	{
		return std::abs(lhs - rhs) < 0.001f;
	}

	constexpr ShittyUnitTest1()
	{
		const auto middle = s_curve_from_input_c(0.5f, 0.5f);
		ASSERT(feq(middle.s, 1.0f) && feq(middle.t, 0.75f));

		const auto top_right = s_curve_from_input_c(1.0f, 1.0f);
		ASSERT(feq(top_right.s, 243.0f) && feq(top_right.t, 0.5f));

		const auto bottom_right = s_curve_from_input_c(1.0f, 0.0f);
		ASSERT(feq(bottom_right.s, 243.0f) && feq(bottom_right.t, 1.0f));
	}
} shitty_unit_test1;
#endif

}  //  namespace klotter
