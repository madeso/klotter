#pragma once

#include "klotter/render/vertex_layout.h"

namespace klotter
{

struct ShaderOptions
{
	bool use_texture = false;
	bool use_lights = false;
	int number_of_point_lights = 0;
	int number_of_frustum_lights = 0;
	int number_of_directional_lights = 0;

	bool transparent_cutoff = false;
	ShaderOptions with_transparent_cutoff() const;
};

struct ShaderSource
{
	ShaderVertexAttributes layout;
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

ShaderSource load_shader_source(
	const ShaderOptions& options, const std::string& uniform_buffer_source
);

}  //  namespace klotter
