#include "klotter/render/ui.h"

#include "klotter/str.h"
#include "klotter/cint.h"

#include "klotter/render/texture.h"

#include <iostream>

namespace klotter
{

// opengl code copied from the imgui opengl3 backend with minor modifications
// todo(Gustav): should we use the imgui backend code for "backend" rendering or use our own shader?

static bool check_imgui_shader(GLuint handle, const char* desc)
{
	GLint status = 0;
	GLint log_length = 0;

	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
	if (status == GL_FALSE)
	{
		std::cerr << "ERROR: Shader code: failed to compile " << desc << "!\n";
	}

	if (log_length > 1)
	{
		std::vector<GLchar> buf;
		buf.resize(sizet_from_int(log_length + 1));
		glGetShaderInfoLog(handle, log_length, nullptr, buf.data());
		std::cerr << buf.data();
	}
	return status == GL_TRUE;
}

static bool imgui_check_program(GLuint handle, const char* desc)
{
	GLint status = 0;
	GLint log_length = 0;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
	if (status == GL_FALSE)
	{
		std::cerr << "ERROR: Shader program: failed to link " << desc << "!\n";
	}

	if (log_length > 1)
	{
		std::vector<GLchar> buf;
		buf.resize(sizet_from_int(log_length + 1));
		glGetProgramInfoLog(handle, log_length, nullptr, buf.data());
		std::cerr << buf.data();
	}
	return status == GL_TRUE;
}

void imgui_destroy_shader(ImguiShaderProgram* bd)
{
	if (bd->program_handle == 0)
	{
		return;
	}

	glDeleteProgram(bd->program_handle);
	bd->program_handle = 0;
}

ImguiShaderProgram imgui_load_shader(const char* glsl_version_string, const char* vertex_shader, const char* fragment_shader)
{
	// Create shaders
	const GLchar* vertex_shader_with_version[2] = {glsl_version_string, vertex_shader};
	const auto vert_handle = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_handle, 2, vertex_shader_with_version, nullptr);
	glCompileShader(vert_handle);
	if (! check_imgui_shader(vert_handle, "vertex shader"))
	{
		return {};
	}

	const GLchar* fragment_shader_with_version[2] = {glsl_version_string, fragment_shader};
	const auto frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_handle, 2, fragment_shader_with_version, nullptr);
	glCompileShader(frag_handle);

	if (! check_imgui_shader(frag_handle, "fragment shader"))
	{
		return {};
	}

	// Link
	ImguiShaderProgram prog;
	prog.program_handle = glCreateProgram();
	glAttachShader(prog.program_handle, vert_handle);
	glAttachShader(prog.program_handle, frag_handle);
	glLinkProgram(prog.program_handle);
	if (! imgui_check_program(prog.program_handle, "shader program"))
	{
		imgui_destroy_shader(&prog);
		return {};
	}

	glDetachShader(prog.program_handle, vert_handle);
	glDetachShader(prog.program_handle, frag_handle);
	glDeleteShader(vert_handle);
	glDeleteShader(frag_handle);

	prog.texture_attrib = glGetUniformLocation(prog.program_handle, "Texture");
	prog.projection_attrib = glGetUniformLocation(prog.program_handle, "ProjMtx");
	return prog;
}

constexpr auto dear_imgui_shader_version = "#version 330 core\n";

constexpr auto linear_to_gamma_glsl_vert = R"glsl(
layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;
uniform mat4 ProjMtx;
out vec2 Frag_UV;
out vec4 Frag_Color;
void main()
{
    Frag_UV = UV;
    Frag_Color = Color;
    gl_Position = ProjMtx * vec4(Position.xy,0,1);
}
)glsl";

constexpr auto linear_to_gamma_glsl_frag = R"glsl(
in vec2 Frag_UV;
in vec4 Frag_Color;
uniform sampler2D Texture;
layout (location = 0) out vec4 Out_Color;
void main()
{
    vec4 sample = texture(Texture, Frag_UV.st);
	vec3 color = sample.rgb / (sample.rgb + vec3(1.0f)); // reinhard tone mapping
	float gamma = 2.2f;
	vec3 gamma_corrected = pow(color.rgb, vec3(1.0f/gamma));
    Out_Color = Frag_Color * vec4(gamma_corrected, sample.a);
}
)glsl";


ImguiShaderCache::ImguiShaderCache()
	: linear_to_gamma_shader(imgui_load_shader(dear_imgui_shader_version,
		linear_to_gamma_glsl_vert, linear_to_gamma_glsl_frag))
{
}

ImguiShaderCache::~ImguiShaderCache()
{
	imgui_destroy_shader(&linear_to_gamma_shader);
}


void imgui_set_shared_shader_params(ImguiShaderProgram* prog)
{
	ImDrawData* draw_data = ImGui::GetDrawData();

	const auto le = draw_data->DisplayPos.x;
	const auto ri = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	const auto to = draw_data->DisplayPos.y;
	const auto bo = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

	const float ortho_projection[4][4] = {
		{2.0f / (ri - le), 0.0f, 0.0f, 0.0f},
		{0.0f, 2.0f / (to - bo), 0.0f, 0.0f},
		{0.0f, 0.0f, -1.0f, 0.0f},
		{(ri + le) / (le - ri), (to + bo) / (bo - to), 0.0f, 1.0f},
	};
	glUseProgram(prog->program_handle);
	glUniform1i(prog->texture_attrib, 0);
	glUniformMatrix4fv(prog->projection_attrib, 1, GL_FALSE, &ortho_projection[0][0]);
}

void im_draw_callback_linear_to_gamma(const ImDrawList*, const ImDrawCmd* cmd)
{
	auto* prog = static_cast<ImguiShaderProgram*>(cmd->UserCallbackData);

	imgui_set_shared_shader_params(prog);
}


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



void imgui_text(const std::string& str)
{
    ImGui::Text("%s", str.c_str());
}



static void draw_imgui_image(const FrameBuffer& img, const ImVec2& image_size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& border_col, ImguiShaderCache* cache, ImageShader shader)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// set shader
	switch (shader)
	{
	case ImageShader::TonemapAndGamma:
		draw_list->AddCallback(im_draw_callback_linear_to_gamma, &cache->linear_to_gamma_shader);
		break;
	case ImageShader::None:
		// no shader
		break;
	}

	// draw image
	ImGui::ImageWithBg(img.id, image_size, uv0, uv1, border_col);

	// reset shader
	if (shader != ImageShader::None)
	{
		draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
	}
}



static void image_tooltip(const FrameBuffer& texture_id, const ImVec2 texture_size, const float& region_size, const float& hover_size,
	const ImVec2 mouse_pos, const ImVec2 widget_size, const ImVec2 pos, const ImVec4 border_col, ImguiShaderCache* cache, ImageShader shader)
{
	const auto region = calculate_region(mouse_pos, pos, texture_size, widget_size, region_size);
	const auto flipped_region_y = texture_size.y - region.y;
	const auto uv0 = ImVec2{region.x / texture_size.x, (flipped_region_y) / texture_size.y};
	const auto uv1 = ImVec2{(region.x + region_size) / texture_size.x, (flipped_region_y - region_size) / texture_size.y};

	// todo(Gustav): can we display pixel value instead of where we are looking? is the region important information?
	imgui_text(Str{} << "UL: " << region.x << " " << region.y);
	imgui_text(Str{} << "LR: " << region.x + region_size << " " << region.y + region_size);
	draw_imgui_image(texture_id, ImVec2(hover_size, hover_size), uv0, uv1, border_col, cache, shader);
}



void imgui_image(const char* name, const FrameBuffer& img, ImguiShaderCache* cache, ImageShader shader)
{
	const auto texture_size = ImVec2{static_cast<float>(img.size.x), static_cast<float>(img.size.y)};

	// todo(Gustav): make the arguments widget_size and zoom level AND make them configurable (with scrolling)
	static float widget_height = 100.0f;
	static float region_size = 32.0f;
	static float hover_size = 128.0f;
	const auto& io = ImGui::GetIO();

	imgui_text(Str{} << name << ": " << texture_size.x << "x" << texture_size.y);

	const auto widget_width = (texture_size.x / texture_size.y) * widget_height;
	const auto widget_size = ImVec2{widget_width, widget_height};

	const auto pos = ImGui::GetCursorScreenPos();
	constexpr auto uv_min = ImVec2{0.0f, 1.0f};	 // Top-left
	constexpr auto uv_max = ImVec2{1.0f, 0.0f};	 // Lower-right
	const auto border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

	static ImVec2 latest_tooltip;
	static ImGuiID current_id = 0;

	const constexpr char* const popup_id = "image config popup";

	draw_imgui_image(img, widget_size, uv_min, uv_max, border_col, cache, shader);
	const auto id = ImGui::GetID(name);

	if (id == current_id && ImGui::BeginPopupContextItem(popup_id))
	{
		ImGui::DragFloat("Base", &widget_height, 1.0f);
		ImGui::DragFloat("Size", &region_size, 0.01f);
		ImGui::DragFloat("Scale", &hover_size, 1.0f);
		image_tooltip(img, texture_size, region_size, hover_size, latest_tooltip, widget_size, pos, border_col, cache, shader);
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::OpenPopupOnItemClick(popup_id, ImGuiPopupFlags_MouseButtonRight);

	// todo(Gustav): look into SetItemTooltip and BeginItemTooltip from https://github.com/ocornut/imgui/releases/tag/v1.89.7
	if (ImGui::IsItemHovered())
	{
		current_id = id;
		latest_tooltip = io.MousePos;
		if (ImGui::BeginTooltip())
		{
			image_tooltip(img, texture_size, region_size, hover_size, io.MousePos, widget_size, pos, border_col, cache, shader);
			ImGui::EndTooltip();
		}
	}
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

