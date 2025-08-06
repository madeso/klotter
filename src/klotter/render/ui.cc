#include "klotter/render/ui.h"

#include "klotter/render/texture.h"

namespace klotter
{



ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y};
}



ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return {lhs.x - rhs.x, lhs.y - rhs.y};
}



ImVec2 calculate_region(const ImVec2& mouse_pos, const ImVec2& pos, const ImVec2& my_tex, const ImVec2& widget_size, float region_sz)
{
	float region_x = (mouse_pos.x - pos.x) * (my_tex.x / widget_size.x) - region_sz * 0.5f;
	float region_y = (mouse_pos.y - pos.y) * (my_tex.y / widget_size.y) - region_sz * 0.5f;
	if (region_x < 0.0f)
	{
		region_x = 0.0f;
	}
	else if (region_x > my_tex.x - region_sz)
	{
		region_x = my_tex.x - region_sz;
	}
	if (region_y < 0.0f)
	{
		region_y = 0.0f;
	}
	else if (region_y > my_tex.y - region_sz)
	{
		region_y = my_tex.y - region_sz;
	}

	return {region_x, region_y};
}



static void imgui_image(ImTextureID texture_id, const ImVec2 texture_size)
{
	// todo(Gustav): make the arguments widget_size and zoom level AND make them configurable (with scrolling)
	constexpr float region_size = 32.0f;
	constexpr float hover_scale = 4.0f;
	const auto& io = ImGui::GetIO();

	ImGui::Text("%.0fx%.0f", texture_size.x, texture_size.y);

	const auto max_size = ImGui::GetContentRegionAvail();
	const auto scale = std::min(max_size.x / texture_size.x, max_size.y / texture_size.y);
	const auto widget_size = ImVec2
	{
		std::max(10.0f, texture_size.x * scale),
		std::max(10.0f, texture_size.y * scale)
	};

	const auto pos = ImGui::GetCursorScreenPos();
	constexpr auto uv_min = ImVec2{0.0f, 1.0f};	 // Top-left
	constexpr auto uv_max = ImVec2{1.0f, 0.0f};	 // Lower-right
	constexpr auto tint_col = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
	const auto border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
	ImGui::Image(texture_id, widget_size, uv_min, uv_max, tint_col, border_col);
	if (ImGui::IsItemHovered())
	{
		const auto region = calculate_region(io.MousePos, pos, texture_size, widget_size, region_size);
		const auto flipped_region_y = texture_size.y - region.y;
		const auto uv0 = ImVec2{region.x / texture_size.x, (flipped_region_y + region_size) / texture_size.y};
		const auto uv1 = ImVec2{(region.x + region_size) / texture_size.x, (flipped_region_y) / texture_size.y};

		ImGui::BeginTooltip();
		// todo(Gustav): can we display pixel value instead of where we are looking? is the region important information?
		ImGui::Text("Min: (%.2f, %.2f)", region.x, region.y);
		ImGui::Text("Max: (%.2f, %.2f)", region.x + region_size, region.y + region_size);
		ImGui::Image(texture_id, ImVec2(region_size * hover_scale, region_size * hover_scale), uv0, uv1, tint_col, border_col);
		ImGui::EndTooltip();
	}
}



static ImTextureID imgui_texture_from(unsigned int texture)
{
	std::int64_t id = texture;
	return reinterpret_cast<ImTextureID>(id);
}



void imgui_image(const FrameBuffer& img)
{
	imgui_image(imgui_texture_from(img.id), {static_cast<float>(img.width), static_cast<float>(img.height)});
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

