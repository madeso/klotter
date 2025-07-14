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
	const auto slope = a(10 * (q0 - 0.5f));
	const auto threshold = 0.5f + (0.5f * (1.0f - q1));
	return {slope, threshold};
}

float calculate_s_curve(float x, float slope, float threshold)
{
	constexpr float machine_epsilon = 0.00001f;
	return x < threshold ?
		threshold * x / (x + slope * (threshold - x) + machine_epsilon) :
		((1 - threshold) * (x - 1)) / (1 - x - slope * (threshold - x) + machine_epsilon) + 1
	;
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
	ImGui::Text("%s (%f %f)", title, static_cast<double>(scd->curve.slope), static_cast<double>(scd->curve.threshold));
	if (ImGui::BeginChild(title, ImVec2{100, 100}, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)
		== false)
	{
		ImGui::EndChild();
		return false;
	}

	// todo(Gustav)L move to a settings struct
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
	for (std::size_t point_index = 0; point_index < max_points + 1; point_index += 1)
	{
		const float srcx = static_cast<float>(point_index) / static_cast<float>(max_points);
		const float x = flip_x ? 1 - srcx : srcx;
		const float y = calculate_s_curve(srcx, scd->curve.slope, scd->curve.threshold);
		points[point_index] = ImVec2{x * size.x, (1 - y) * size.y} + pos;
	}
	draw->AddPolyline(points.data(), max_points + 1, line_color, ImDrawFlags_None, 1.0f);

	if (draw_points)
	{
		for (std::size_t point_index = 0; point_index < max_points + 1; point_index += 1)
		{
			draw->AddCircleFilled(points[point_index], dot_radius, dot_color);
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
		if ((s) < 0.0f || (s) > 1.0f) \
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

}  //  namespace klotter
