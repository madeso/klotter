#include "klotter/scene.h"
#include "klotter/render/mesh.builder.h"

#include <cmath>

using namespace klotter;

void register_cube(SceneApp* app);
void register_scene(SceneApp* app);
void register_lights(SceneApp* app);

struct ExampleApp : SceneApp
{
    ExampleApp()
    {
        register_cube(this);
        register_scene(this);
        register_lights(this);
    }
};

IMPLEMENT_MAIN(ExampleApp)
