#pragma once

#include "klotter/render/vertex_layout.h"

namespace klotter
{

struct ShaderOptions
{
	bool use_blinn_phong = true;
	bool use_texture = false;
	bool use_lights = false;

	int number_of_point_lights = 0;
	int number_of_frustum_lights = 0;
	int number_of_directional_lights = 0;

	bool transparent_cutoff = false;
	[[nodiscard]] ShaderOptions with_transparent_cutoff() const;

	bool use_instancing = false;
	[[nodiscard]] ShaderOptions with_instanced_mat4() const;
};

struct VertexShaderSource
{
	ShaderVertexAttributes layout;
	std::string vertex;
	std::string fragment;
};

struct ShaderSource
{
	std::string vertex;
	std::string fragment;
};

enum class BlurType
{
	horizontal,
	vertical
};

enum class IsGauss
{
	no,
	yes
};

struct BlurOptions
{
	BlurType blur;
	int sample_count;
	IsGauss is_gauss;
};

std::string generate_blur(std::string_view src, const BlurOptions& options);

VertexShaderSource load_shader_source(const ShaderOptions& options, const std::string& uniform_buffer_source);

ShaderSource load_skybox_source(const std::string& uniform_buffer_source);

}  //  namespace klotter
