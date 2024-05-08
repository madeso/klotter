#include "klotter/render/geom.builder.h"
#include "sample.h"

#include <cmath>

using namespace klotter;

namespace examples
{
void add_sample_cube(SampleApp* app);
void add_sample_scene(SampleApp* app);
void add_sample_lights(SampleApp* app);
void add_sample_transform(SampleApp* app);
}  //  namespace examples

struct ExampleApp : examples::SampleApp
{
	static klotter::RenderSettings get_render_settings()
	{
		// use default
		return klotter::RenderSettings{};
	}

	explicit ExampleApp(klotter::Renderer*)
	{
		add_sample_cube(this);
		add_sample_scene(this);
		add_sample_transform(this);
		add_sample_lights(this);
	}
};

IMPLEMENT_MAIN(ExampleApp)
