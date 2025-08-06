#pragma once
#include "imgui.h"

namespace klotter
{


struct FrameBuffer;


ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs);
ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);

/// exposed for testing purposes
ImVec2 calculate_region(const ImVec2& mouse_pos, const ImVec2& pos, const ImVec2& my_tex, const ImVec2& widget_size, float region_sz);

void imgui_image(const FrameBuffer& img);
bool simple_gamma_slider(const char* label, float* gamma, float curve = 1.5f, float min_gamma = 1.6f, float max_gamma = 2.8f);

}
