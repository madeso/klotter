#include "klotter/imgui.theme.h"

#include "klotter/render/color.h"

#include "imgui.h"

namespace klotter
{

#if FF_HAS(ENABLE_THEMES)


// current system is inspired by https://www.youtube.com/watch?v=vvPklRN0Tco

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

float clip_float(float f)
{
	if (f <= 0) return 0;
	if (f >= 1) return 1;
	return f;
}

Lin_rgb clip_lin(Lin_rgb c)
{
	const auto ret = Lin_rgb {{clip_float(c.linear.r), clip_float(c.linear.g), clip_float(c.linear.b)}};
	return ret;
}

struct GuiColor
{
	float chroma;
	Angle hue;
};

ImVec4 imgui_from_lch(float lightness, const GuiColor& color)
{
	const auto lch = Lch{.l = lightness, .c = color.chroma, .h = color.hue};
	const auto lab = oklab_from_oklch(lch);
	const auto lin = linear_from_oklab(lab);
	// const auto lin_pre = gamut_clip_preserve_chroma(lin);
	const auto lin_pre = clip_lin(lin);
	const auto rgb = srgb_from_linear(lin_pre);
	const auto ret = ImVec4{rgb.r, rgb.g, rgb.b, 1.0f};
	return ret;
}

void setup_custom_theme(const GuiColor& common, const GuiColor& histogram)
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
	style.TabBarOverlineSize = 2.0f;

	// // bkg
	
	auto bg_dark = imgui_from_lch(0.1f, common);
	auto bg = imgui_from_lch(0.2f, common);
	auto bg_light = imgui_from_lch(0.3f, common);
	auto bg_lighter = imgui_from_lch(0.4f, common);

	auto text = imgui_from_lch(0.95f, common);
	auto text_muted = imgui_from_lch(0.70f, common);

	auto interactive = imgui_from_lch(0.50f, common);

	auto hist_default = imgui_from_lch(0.80f, histogram);
	auto hist_interactive = imgui_from_lch(0.91f, histogram);
	
	
	style.Colors[ImGuiCol_Text] = text;
	style.Colors[ImGuiCol_TextDisabled] = text_muted;
	style.Colors[ImGuiCol_WindowBg] = bg_dark; // Background of normal windows
	style.Colors[ImGuiCol_ChildBg] = bg_dark; // Background of child windows
	style.Colors[ImGuiCol_PopupBg] = bg_dark; // Background of popups, menus, tooltips windows
	style.Colors[ImGuiCol_Border] = bg_light;
	style.Colors[ImGuiCol_BorderShadow] = bg_light;
	style.Colors[ImGuiCol_FrameBg] = bg; // Background of checkbox, radio button, plot, slider, text input
	style.Colors[ImGuiCol_FrameBgHovered] = bg_light;
	style.Colors[ImGuiCol_FrameBgActive] = bg_light;
	style.Colors[ImGuiCol_TitleBg] = bg_dark; // Title bar
	style.Colors[ImGuiCol_TitleBgActive] = bg_light; // Title bar when focused
	style.Colors[ImGuiCol_TitleBgCollapsed] = bg_dark; // Title bar when collapsed
	style.Colors[ImGuiCol_MenuBarBg] = bg_light;
	
	style.Colors[ImGuiCol_ScrollbarBg] = bg_light;
	style.Colors[ImGuiCol_ScrollbarGrab] = bg;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = interactive;
	style.Colors[ImGuiCol_ScrollbarGrabActive] = interactive;
	style.Colors[ImGuiCol_CheckMark] = text;             // Checkbox tick and RadioButton circle
	style.Colors[ImGuiCol_SliderGrab] = bg_lighter;
	style.Colors[ImGuiCol_SliderGrabActive] = interactive;
	style.Colors[ImGuiCol_Button] = bg_light;
	style.Colors[ImGuiCol_ButtonHovered] = interactive;
	style.Colors[ImGuiCol_ButtonActive] = interactive;
	style.Colors[ImGuiCol_Header] = bg_light;                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
	style.Colors[ImGuiCol_HeaderHovered] = interactive;
	style.Colors[ImGuiCol_HeaderActive] = interactive;
	style.Colors[ImGuiCol_Separator] = bg_light;
	style.Colors[ImGuiCol_SeparatorHovered] = interactive; // sample: table columns, resizing
	style.Colors[ImGuiCol_SeparatorActive] = interactive;
	style.Colors[ImGuiCol_ResizeGrip] = bg_light; // Resize grip in lower-right and lower-left corners of windows.
	style.Colors[ImGuiCol_ResizeGripHovered] = interactive;
	style.Colors[ImGuiCol_ResizeGripActive] = interactive;
	style.Colors[ImGuiCol_InputTextCursor] = text;       // InputText cursor/caret

	// tab overline is only displayed when the tab that is requested
	style.Colors[ImGuiCol_TabHovered] = interactive;            // Tab background, when hovered
	style.Colors[ImGuiCol_Tab] = bg;                   // Tab background, when tab-bar is focused & tab is unselected
	style.Colors[ImGuiCol_TabSelected] = bg_lighter;           // Tab background, when tab-bar is focused & tab is selected
	style.Colors[ImGuiCol_TabSelectedOverline] = text_muted;   // Tab horizontal overline, when tab-bar is focused & tab is selected

	// is dimmed tab possible?
	style.Colors[ImGuiCol_TabDimmed] = bg;	// Tab background, when tab-bar is unfocused & tab is unselected
	style.Colors[ImGuiCol_TabDimmedSelected] = bg_lighter;  // Tab background, when tab-bar is unfocused & tab is selected
	style.Colors[ImGuiCol_TabDimmedSelectedOverline] = text_muted;  //..horizontal overline, when tab-bar is unfocused & tab is selected

	style.Colors[ImGuiCol_PlotLines] = text_muted;
	style.Colors[ImGuiCol_PlotLinesHovered] = interactive;
	style.Colors[ImGuiCol_PlotHistogram] = hist_default;
	style.Colors[ImGuiCol_PlotHistogramHovered] = hist_interactive;
	
	style.Colors[ImGuiCol_TableHeaderBg] = bg_light;         // Table header background
	style.Colors[ImGuiCol_TableBorderStrong] = interactive;     // Table outer and header borders (prefer using Alpha=1.0 here)
	style.Colors[ImGuiCol_TableBorderLight] = bg_light;      // Table inner borders (prefer using Alpha=1.0 here)
	style.Colors[ImGuiCol_TableRowBg] = bg_dark;            // Table row background (even rows)
	style.Colors[ImGuiCol_TableRowBgAlt] = bg;         // Table row background (odd rows)
	
	// style.Colors[ImGuiCol_TextLink] = color;              // Hyperlink color
	style.Colors[ImGuiCol_TextSelectedBg] = bg_lighter;        // Selected text inside an InputText
	style.Colors[ImGuiCol_TreeLines] = interactive;             // Tree node hierarchy outlines when using ImGuiTreeNodeFlags_DrawLines
	style.Colors[ImGuiCol_DragDropTarget] = hist_interactive;        // Rectangle highlighting a drop target
	style.Colors[ImGuiCol_UnsavedMarker] = text;         // Unsaved Document marker (in window title and tabs)
	// style.Colors[ImGuiCol_NavCursor] = color;             // Color of keyboard/gamepad navigation cursor/rectangle, when visible
	// style.Colors[ImGuiCol_NavWindowingHighlight] = color; // Highlight window when using CTRL+TAB
	// style.Colors[ImGuiCol_NavWindowingDimBg] = color;     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
	// style.Colors[ImGuiCol_ModalWindowDimBg] = color;      // Darken/colorize entire screen behind a modal window, when one is active
}

void test_themes()
{
	static GuiColor common = {0.0f, Angle::from_degrees(214.0f)};

	static GuiColor histogram = {0.18f, Angle::from_degrees(110.0f)};

	static bool changed = true;
	changed = ImGui::SliderFloat("Chroma", &common.chroma, 0.0f, 0.05f) || changed;
	changed = ImGui::SliderAngle("Hue", &common.hue.radians, 0.0f) || changed;

	changed = ImGui::SliderFloat("Hist Chroma", &histogram.chroma, 0.0f, 0.8f) || changed;
	changed = ImGui::SliderAngle("Hist Hue", &histogram.hue.radians, 0.0f) || changed;

	if (ImGui::Button("setup"))
	{
		changed = true;
	}

	if (changed)
	{
		setup_custom_theme(common, histogram);
		changed = false;
	}
}

#else

void test_themes()
{
}

#endif

}  //  namespace klotter

