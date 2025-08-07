#include "klotter/assert.h"

#include "catch2/catch_session.hpp"

int main(int argc, char* argv[])
{
	enable_exception_on_assert();
	return Catch::Session().run(argc, argv);
}
