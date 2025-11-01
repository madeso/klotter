#pragma once
#include "imgui.h"

namespace klotter
{


struct FrameBuffer;

struct ImguiShaderProgram
{
	unsigned int program_handle = 0;
	int texture_attrib = 0;
	int projection_attrib = 0;
};

struct ImguiShaderCache
{
	ImguiShaderCache();
	~ImguiShaderCache();

	ImguiShaderCache(const ImguiShaderCache&) = delete;
	ImguiShaderCache(ImguiShaderCache&&) = delete;
	ImguiShaderCache& operator=(const ImguiShaderCache&) = delete;
	ImguiShaderCache& operator=(ImguiShaderCache&&) = delete;

	ImguiShaderProgram linear_to_gamma_shader;
	ImguiShaderProgram depth_ortho_shader;
};

enum class ImageShader
{
	None,
	TonemapAndGamma,
	DepthOrtho
};


ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs);
ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);

/// exposed for testing purposes
ImVec2 calculate_region(const ImVec2& mouse_pos, const ImVec2& pos, const ImVec2& my_tex, const ImVec2& widget_size, float region_sz);

void imgui_text(const std::string& str);
void imgui_image(const char* name, const FrameBuffer& img, ImguiShaderCache* cache, ImageShader shader);
bool simple_gamma_slider(const char* label, float* gamma, float curve = 1.5f, float min_gamma = 1.6f, float max_gamma = 2.8f);

}
