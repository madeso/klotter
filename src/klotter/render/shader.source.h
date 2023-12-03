#pragma once

#include "klotter/render/vertex_layout.h"

namespace klotter
{

struct ShaderOptions
{
	bool use_lights = false;
	int number_of_point_lights = 0;
};

struct ShaderSource
{
	ShaderVertexAttributes layout;
	std::string vertex;
	std::string fragment;
};

ShaderSource load_shader_source(const ShaderOptions& options);

}  //  namespace klotter
