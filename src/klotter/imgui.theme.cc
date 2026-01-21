

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

void test_themes()
{
	std::vector<ThemeColor*> colors;
	
	#define USE_COLOR(name) \
		do \
		{ \
			static ThemeColor color = { #name, {1.0f, 0.0f, Angle::from_degrees(50.0f)} };\
			colors.emplace_back(&color);\
		} while(false)
	// bkg
	USE_COLOR(bg_dark);
	USE_COLOR(bg);
	USE_COLOR(bg_light);
	USE_COLOR(border);
	// text
	USE_COLOR(text);
	USE_COLOR(text_muted);
	// buttons
	USE_COLOR(primary);
	USE_COLOR(secondary);
	// messages
	USE_COLOR(danger);
	USE_COLOR(warning);
	USE_COLOR(success);
	USE_COLOR(info);
	#undef USE_COLOR

	if (ImGui::BeginTable("Colors", 4))
	{
		// ImGuiTableColumnFlags_
		ImGui::TableSetupColumn("Color");
		ImGui::TableSetupColumn("Light");
		ImGui::TableSetupColumn("Chroma");
		ImGui::TableSetupColumn("Hue");
		ImGui::TableHeadersRow();
		for (const auto& c : colors)
		{
			ImGui::PushID(c);

			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::TextUnformatted(c->name.c_str());

			ImGui::TableNextColumn();
			ImGui::SliderFloat("##l", &c->color.l, 0.0f, 1.0f);

			ImGui::TableNextColumn();
			ImGui::SliderFloat("##c", &c->color.c, 0.0f, 1.0f);

			ImGui::TableNextColumn();
			auto deg = c->color.h.as_degrees();
			if (ImGui::SliderFloat("##h", &deg, 0.0f, 360.0f))
			{
				c->color.h = Angle::from_degrees(deg);
			}

			ImGui::PopID();
		}
		ImGui::EndTable();
	}

	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowRounding = 8.0f;
	style.ChildRounding = 8.0f;
	style.FrameRounding = 6.0f;
	style.PopupRounding = 6.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabRounding = 6.0f;
	style.TabRounding = 6.0f;

	
	std::vector<SelectedTheme> selections;
	#define USE_THEME(x) \
		do \
		{ \
		static int sel = -1;\
		selections.emplace_back(SelectedTheme { #x, x, &sel});\
		} while (false)
	USE_THEME(ImGuiCol_Text);
	USE_THEME(ImGuiCol_TextDisabled);
	USE_THEME(ImGuiCol_WindowBg);
	USE_THEME(ImGuiCol_ChildBg);
	USE_THEME(ImGuiCol_PopupBg);
	USE_THEME(ImGuiCol_Border);
	USE_THEME(ImGuiCol_FrameBg);
	USE_THEME(ImGuiCol_FrameBgHovered);
	USE_THEME(ImGuiCol_TitleBg);
	USE_THEME(ImGuiCol_TitleBgActive);
	USE_THEME(ImGuiCol_TitleBgCollapsed);
	USE_THEME(ImGuiCol_MenuBarBg);
	USE_THEME(ImGuiCol_ScrollbarGrabActive);
	USE_THEME(ImGuiCol_CheckMark);
	USE_THEME(ImGuiCol_SliderGrab);
	USE_THEME(ImGuiCol_Header);
	USE_THEME(ImGuiCol_HeaderHovered);
	USE_THEME(ImGuiCol_SeparatorActive);
	USE_THEME(ImGuiCol_ResizeGrip);
	USE_THEME(ImGuiCol_ResizeGripHovered);
	USE_THEME(ImGuiCol_InputTextCursor);
	USE_THEME(ImGuiCol_TabHovered);
	USE_THEME(ImGuiCol_Tab);
	USE_THEME(ImGuiCol_TabSelected);
	USE_THEME(ImGuiCol_TabSelectedOverline);
	USE_THEME(ImGuiCol_TabDimmed);
	USE_THEME(ImGuiCol_TabDimmedSelected);
	USE_THEME(ImGuiCol_TabDimmedSelectedOverline);
	USE_THEME(ImGuiCol_PlotLines);
	USE_THEME(ImGuiCol_TableHeaderBg);
	USE_THEME(ImGuiCol_TableBorderStrong);
	USE_THEME(ImGuiCol_TableBorderLight);
	USE_THEME(ImGuiCol_TableRowBg);
	USE_THEME(ImGuiCol_TableRowBgAlt);
	USE_THEME(ImGuiCol_TextLink);
	USE_THEME(ImGuiCol_TextSelectedBg);
	USE_THEME(ImGuiCol_TreeLines);
	USE_THEME(ImGuiCol_DragDropTarget);
	USE_THEME(ImGuiCol_UnsavedMarker);
	USE_THEME(ImGuiCol_NavCursor);
	USE_THEME(ImGuiCol_NavWindowingHighlight);
	USE_THEME(ImGuiCol_NavWindowingDimBg);
	USE_THEME(ImGuiCol_ModalWindowDimBg);
	#undef USE_THEME

	for (const auto& s: selections)
	{
		ImGui::PushID(s.selection);
		if (ImGui::BeginCombo(s.name.c_str(), *s.selection != -1 ? colors[*s.selection]->name.c_str() : "???"))
		{
			int id = 0;
			for (const auto& c: colors)
			{
				bool selected = *s.selection == id;
				if (ImGui::Selectable(c->name.c_str(), &selected))
				{
					*s.selection = id;
				}
				id += 1;
			}
			ImGui::EndCombo();
		}
		ImGui::PopID();

		if (*s.selection != -1)
		{
			const auto& lch = colors[*s.selection]->color;
			const auto lab = oklab_from_oklch(lch);
			const auto lin = linear_from_oklab(lab);
			const auto rgb = srgb_from_linear(lin);
			style.Colors[s.id] = ImVec4(rgb.r, rgb.g, rgb.b, 1.0f);
		}
	}
}

#else

void test_themes()
{
}

#endif

}  //  namespace klotter

