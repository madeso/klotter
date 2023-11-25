#pragma once

#include "klotter/render/vertex_layout.h"

namespace klotter
{

struct ShaderOptions
{
	bool use_lights = false;
};

struct ShaderSource
{
	ShaderVertexAttributes layout;
	std::string vertex;
	std::string fragment;
};

ShaderSource load_shader_source(const ShaderOptions& options);

}  //  namespace klotter
