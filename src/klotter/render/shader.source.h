#pragma once

#include "klotter/render/vertex_layout.h"

namespace klotter
{


struct ShaderSource
{
	ShaderVertexAttributes layout;
	std::string_view vertex;
	std::string_view fragment;
};

ShaderSource basic_shader_source();
ShaderSource light_shader_source();

}  //  namespace klotter
