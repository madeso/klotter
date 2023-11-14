#include "klotter/render/mesh.builder.h"
#include "scene.h"

#include <cmath>

using namespace klotter;

namespace examples
{
void register_cube(SceneApp* app);
void register_scene(SceneApp* app);
void register_lights(SceneApp* app);
}  //  namespace examples

struct ExampleApp : examples::SceneApp
{
	ExampleApp()
	{
		register_cube(this);
		register_scene(this);
		register_lights(this);
	}
};

IMPLEMENT_MAIN(ExampleApp)
