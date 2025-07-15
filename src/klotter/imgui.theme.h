#pragma once

#include "imgui.h"

namespace klotter
{

/// 100 rabbits theme.
/// https://github.com/hundredrabbits/Themes
struct Theme
{
	static Theme default_theme();

	static Theme apollo();
	static Theme battlestation();
	static Theme berry();
	static Theme bigtime();
	static Theme boysenberry();
	static Theme coal();
	static Theme cobalt();
	static Theme commodore();
	static Theme forestlawn();
	static Theme frameio();
	static Theme gameboy();
	static Theme garden();
	static Theme gotham();
	static Theme haxe();
	static Theme isotope();
	static Theme kawaii();
	static Theme laundry();
	static Theme lotus();
	static Theme mahou();
	static Theme marble();
	static Theme murata();
	static Theme muzieca();
	static Theme nightowl();
	static Theme ninetynine();
	static Theme noir();
	static Theme nord();
	static Theme obsidian();
	static Theme op1();
	static Theme orca();
	static Theme pawbin();
	static Theme pico8();
	static Theme polivoks();
	static Theme rainonwires();
	static Theme roguelight();
	static Theme sk();
	static Theme snow();
	static Theme solarised_dark();
	static Theme solarised_light();
	static Theme sonicpi();
	static Theme soyuz();
	static Theme tape();
	static Theme teenage();
	static Theme teletext();
	static Theme vacuui();
	static Theme zenburn();


	ImVec4 background;	///< Application Background.
	ImVec4 f_high;	///< Foreground, high-contrast.
	ImVec4 f_med;  ///< Foreground, medium-contrast.
	ImVec4 f_low;  ///< Foreground, low-contrast.
	ImVec4 f_inv;  ///< Foreground, for modals and overlays.
	ImVec4 b_high;	///< Background, high-contrast.
	ImVec4 b_med;  ///< Background, medium-contrast.
	ImVec4 b_low;  ///< Background, low-contrast.
	ImVec4 b_inv;  ///< Background, for modals and overlays.
};

void test_themes();

void setup_imgui_theme(const Theme& theme);

void setup_imgui_theme_itmago();
void setup_imgui_theme_purple();
void setup_imgui_theme_qy8q();
void setup_imgui_theme_simv0();
void setup_imgui_theme_tonetfal();
void setup_imgui_theme_codz01();
void setup_imgui_theme_wpsimon();
void setup_imgui_theme_adia();

}  //  namespace klotter
