#include "klotter/assert.h"

#include "klotter/str.h"

#include <iostream>

namespace
{
bool& is_exception_enabled()
{
	static bool enabled = false;
	return enabled;
}
}  //  namespace

void on_assert_failure(std::string_view function, std::string_view reason, std::string_view file, int line)
{
	const std::string text = Str{} << file << "(" << line << "): "
								   << "Assertion failed in " << function << ": " << reason;

	if (is_exception_enabled())
	{
		throw text; // NOLINT(misc-throw-by-value-catch-by-reference)
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
