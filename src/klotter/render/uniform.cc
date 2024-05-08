#include "klotter/render/uniform.h"

namespace klotter
{
Uniform::Uniform(const std::string& n, int l, unsigned int sp)
	: name(n)
	, location(l)
	, debug_shader_program(sp)
{
}

bool Uniform::is_valid() const
{
	return location >= 0;
}

}  //  namespace klotter
