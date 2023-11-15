#include "klotter/render/mesh.builder.h"
#include "sample.h"

#include <cmath>

using namespace klotter;

namespace examples
{
void register_cube(SampleApp* app);
void register_scene(SampleApp* app);
void register_lights(SampleApp* app);
}  //  namespace examples

struct ExampleApp : examples::SampleApp
{
	ExampleApp()
	{
		register_cube(this);
		register_scene(this);
		register_lights(this);
	}
};

IMPLEMENT_MAIN(ExampleApp)
