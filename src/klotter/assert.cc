#include "klotter/assert.h"

#include <iostream>

#include "klotter/str.h"

namespace
{
bool& is_exception_enabled()
{
	static bool enabled = false;
	return enabled;
}
}  //  namespace

void on_assert_failure(
	std::string_view function, std::string_view reason, std::string_view file, int line
)
{
	const std::string text = Str{} << file << "(" << line << "): "
								   << "Assertion failed in " << function << ": " << reason;

	if (is_exception_enabled())
	{
		throw text;
	}
	else
	{
		std::cerr << text << std::endl;
		std::abort();
	}
}

void enable_exception_on_assert()
{
	is_exception_enabled() = true;
}
