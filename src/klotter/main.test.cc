#include "catch2/catch_session.hpp"

#include "klotter/assert.h"

int main(int argc, char* argv[])
{
	enable_exception_on_assert();
	return Catch::Session().run(argc, argv);
}
