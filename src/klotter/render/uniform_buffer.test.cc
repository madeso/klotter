#include "catch.hpp"
// #include "catchy/vectorequals.h"
#include "catchy/stringeq.h"

#include "klotter/render/uniform_buffer.h"


using namespace klotter;

TEST_CASE("uniform_buffer_test_simple", "[uniform_buffer]")
{
	UniformBufferSetup setup;

	CompiledUniformProp value;
	CompiledUniformProp vector;
	CompiledUniformProp matrix;
	CompiledUniformProp values;
	CompiledUniformProp boolean;
	CompiledUniformProp integer;

	{
		UniformBufferCompiler compiler;
		compiler.add(&value, UniformType::float_type, "value");
		compiler.add(&vector, UniformType::vec3, "vector");
		compiler.add(&matrix, UniformType::mat4, "matrix");
		compiler.add(&values, UniformType::float_type, "values", 3);
		compiler.add(&boolean, UniformType::bool_type, "boolean");
		compiler.add(&integer, UniformType::int_type, "integer");
		compiler.compile("Test", &setup, 0);
	}

	CHECK(value.offset == 0);
	CHECK(vector.offset == 16);
	CHECK(matrix.offset == 32);
	CHECK(values.offset == 96);
	CHECK(boolean.offset == 144);
	CHECK(integer.offset == 148);
	CHECK(setup.size == 152);
}

TEST_CASE("uniform_buffer_test_basic_mat", "[uniform_buffer]")
{
	UniformBufferSetup setup;
	CompiledUniformProp projection;
	CompiledUniformProp view;

	{
		UniformBufferCompiler compiler;
		compiler.add(&projection, UniformType::mat4, "projection");
		compiler.add(&view, UniformType::mat4, "view");
		compiler.compile("D", &setup, 0);
	}

	CHECK(projection.offset == 0);
	CHECK(view.offset == 64);
	CHECK(setup.size == 128);

	CHECK(catchy::StringEq(
		setup.source,
		"layout (std140) uniform D\n"
		"{\n"
		"\tmat4 projection;\n"
		"\tmat4 view;\n"
		"};\n"
	));
}
