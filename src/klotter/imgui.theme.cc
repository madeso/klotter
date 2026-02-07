#include "klotter/imgui.theme.h"

#include "klotter/render/color.h"

#include "imgui.h"

namespace klotter
{

#if FF_HAS(ENABLE_THEMES)


// current system is inspired byg https://www.youtube.com/watch?v=vvPklRN0Tco

struct ThemeColor
{
	std::string name;
	Lch color;
};

struct SelectedTheme
{
	std::string name;
	int id;
	int* selection;
};

void set_color(ImGuiStyle& style, ImGuiCol_ color, const Lch& lch)
{
	const auto lab = oklab_from_oklch(lch);
	const auto lin = linear_from_oklab(lab);
	const auto rgb = srgb_from_linear(lin);
	style.Colors[color] = ImVec4(rgb.r, rgb.g, rgb.b, 1.0f);
}

void setup_custom_theme()
{
	// themes
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowTitleAlign = {0.5f, 0.5f};
	style.WindowPadding = {10, 8};
	style.FramePadding = {10, 4};
	style.ItemSpacing = {10, 4};
	style.ItemInnerSpacing = {10, 4};
	style.WindowRounding = 8.0f;
	style.ChildRounding = 8.0f;
	style.FrameRounding = 6.0f;
	style.PopupRounding = 6.0f;
	style.ScrollbarRounding = 6.0f;
	style.ScrollbarPadding = 3;
	style.GrabRounding = 6.0f;
	style.TabRounding = 6.0f;

	// // bkg
	const auto light = false;
	const auto chroma = 0.0f;
	const auto hue = Angle::from_degrees(214);

	// bottom
	auto bg_dark = Lch{light ? 0.9f : 0, chroma, hue};
	// middle
	auto bg = Lch{light ? 0.95f : 0.1f, chroma, hue};
	// top
	auto bg_light = Lch{light ? 1.0f : 0.3f, chroma, hue};

	auto border = Lch{0, chroma, hue};
	
	// // text
	// headings
	auto text = Lch{light ? 0.05f : 0.95f, chroma, hue};
	auto text_muted = Lch{light ? 0.30f : 0.70f, chroma, hue};
	
	// // buttons
	auto primary = Lch{0, chroma, hue};
	auto secondary = Lch{0, chroma, hue};
	
	// // messages
	// USE_COLOR(danger);
	// USE_COLOR(warning);
	// USE_COLOR(success);
	// USE_COLOR(info);

	set_color(style, ImGuiCol_Text, text);
	// set_color(style, ImGuiCol_TextDisabled, lch_color);
	set_color(style, ImGuiCol_WindowBg, bg_dark);
	set_color(style, ImGuiCol_ChildBg, bg_dark);
	set_color(style, ImGuiCol_PopupBg, bg_dark);
	
	set_color(style, ImGuiCol_BorderShadow, bg_light);
	set_color(style, ImGuiCol_Border, bg_light);

	set_color(style, ImGuiCol_FrameBg, bg);
	set_color(style, ImGuiCol_FrameBgHovered, bg_light);
	set_color(style, ImGuiCol_FrameBgActive, bg_light);
	
	set_color(style, ImGuiCol_TitleBg, bg_dark);
	set_color(style, ImGuiCol_TitleBgActive, bg_light);
	set_color(style, ImGuiCol_TitleBgCollapsed, bg_dark);

	set_color(style, ImGuiCol_MenuBarBg, bg_light);

	set_color(style, ImGuiCol_Separator, bg_light);
	// set_color(style, ImGuiCol_SeparatorActive, lch_color);

	// set_color(style, ImGuiCol_ScrollbarGrabActive, bg_light);
	// set_color(style, ImGuiCol_CheckMark, lch_color);
	// set_color(style, ImGuiCol_SliderGrab, lch_color);
	// set_color(style, ImGuiCol_Header, lch_color);
	// set_color(style, ImGuiCol_HeaderHovered, lch_color);
	
	set_color(style, ImGuiCol_ResizeGrip, bg_light);
	set_color(style, ImGuiCol_ResizeGripHovered, bg);

	// set_color(style, ImGuiCol_InputTextCursor, lch_color);
	// set_color(style, ImGuiCol_TabHovered, lch_color);
	// set_color(style, ImGuiCol_Tab, lch_color);
	// set_color(style, ImGuiCol_TabSelected, lch_color);
	// set_color(style, ImGuiCol_TabSelectedOverline, lch_color);
	// set_color(style, ImGuiCol_TabDimmed, lch_color);
	// set_color(style, ImGuiCol_TabDimmedSelected, lch_color);
	// set_color(style, ImGuiCol_TabDimmedSelectedOverline, lch_color);
	// set_color(style, ImGuiCol_PlotLines, lch_color);
	// set_color(style, ImGuiCol_TableHeaderBg, lch_color);
	// set_color(style, ImGuiCol_TableBorderStrong, lch_color);
	// set_color(style, ImGuiCol_TableBorderLight, lch_color);
	// set_color(style, ImGuiCol_TableRowBg, lch_color);
	// set_color(style, ImGuiCol_TableRowBgAlt, lch_color);
	// set_color(style, ImGuiCol_TextLink, lch_color);
	// set_color(style, ImGuiCol_TextSelectedBg, lch_color);
	// set_color(style, ImGuiCol_TreeLines, lch_color);
	// set_color(style, ImGuiCol_DragDropTarget, lch_color);
	// set_color(style, ImGuiCol_UnsavedMarker, lch_color);
	// set_color(style, ImGuiCol_NavCursor, lch_color);
	// set_color(style, ImGuiCol_NavWindowingHighlight, lch_color);
	// set_color(style, ImGuiCol_NavWindowingDimBg, lch_color);
	// set_color(style, ImGuiCol_ModalWindowDimBg, lch_color);
}

void test_themes()
{
	if (ImGui::Button("setup"))
	{
		setup_custom_theme();
	}
}

#else

void test_themes()
{
}

#endif

}  //  namespace klotter

