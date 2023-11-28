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

constexpr SCurve s_curve_from_input_c(float x, float y)
{
	// s curve
	// Colugo's curve editor: https://twitter.com/ColugoMusic/status/1363071439679729665?s=20
	// https://www.desmos.com/calculator/ibek4vkdiw
	// which is based off of Yann van der Cruyssen's sCurve formula: https://twitter.com/Morusque/status/1352569197499441155
	// which is based off of this paper: https://arxiv.org/pdf/2010.09714.pdf

	const auto p0 = x;
	// gustav addition: switch p1 to match curve to xy
	const auto p1 = x > 0.5f ? y : 1 - y;

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

SCurve s_curve_from_input(float x, float y)
{
	return s_curve_from_input_c(x, y);
}

float calculate_s_curve(float x, float s, float t)
{
	constexpr float mE = 0.00001f;	// machine epsilon
	return x < t ? t * x / (x + s * (t - x) + mE)
				 : ((1 - t) * (x - 1)) / (1 - x - s * (t - x) + mE) + 1;
}

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y};
}

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return {lhs.x - rhs.x, lhs.y - rhs.y};
}

bool imgui_s_curve_editor(const char* title, SCurve* sc, ImVec2* drag)
{
	ImGui::Text(
		"%s (%f %f/%f %f)",
		title,
		static_cast<double>(drag->x),
		static_cast<double>(drag->y),
		static_cast<double>(sc->s),
		static_cast<double>(sc->t)
	);
	if (ImGui::BeginChild(
			title, ImVec2{100, 100}, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove
		)
		== false)
	{
		ImGui::EndChild();
		return false;
	}

	constexpr float radius = 10.0f;
	constexpr float dot_radius = 3.0f;
	constexpr auto button = ImGuiMouseButton_Left;
	constexpr int max_points = 20;
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

	ImVec2 points[max_points + 1];
	for (int i = 0; i < max_points + 1; i += 1)
	{
		const float x = static_cast<float>(i) / static_cast<float>(max_points);
		const float y = calculate_s_curve(x, sc->s, sc->t);
		points[i] = ImVec2{x * size.x, (1 - y) * size.y} + pos;
	}
	draw->AddPolyline(points, max_points + 1, line_color, ImDrawFlags_None, 1.0f);

	if (draw_points)
	{
		for (int i = 0; i < max_points + 1; i += 1)
		{
			draw->AddCircleFilled(points[i], dot_radius, dot_color);
		}
	}
	draw->AddCircleFilled(
		ImVec2{drag->x * size.x, (1 - drag->y) * size.y} + pos, radius, drag_color
	);

	const auto t01 = [&pos, &size](const ImVec2& mp)
	{
		const auto x = (mp.x - pos.x) / size.x;
		const auto y = 1 - ((mp.y - pos.y) / size.y);
		return ImVec2{x, y};
	};

	const auto within01 = [](float s)
	{
		if (s < 0.0f) return false;
		if (s > 1.0f)
			return false;
		else
			return true;
	};

	auto& io = ImGui::GetIO();

	const auto drag_start = t01(io.MouseClickedPos[button]);

	if (ImGui::IsMouseDragging(button) && within01(drag_start.x) && within01(drag_start.y))
	{
		const auto mp = ImGui::GetMousePos();
		const auto n = t01(mp);
		drag->x = std::min(1.0f, std::max(n.x, 0.0f));
		drag->y = std::min(1.0f, std::max(n.y, 0.0f));
		*sc = s_curve_from_input(drag->x, drag->y);
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
