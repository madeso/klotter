#include "klotter/klotter.h"
#include "klotter/render/mesh.builder.h"

#include <cmath>

using namespace klotter;

void register_cube(App* app);
void register_scene(App* app);
void register_lights(App* app);

struct ExampleApp : App
{
    ExampleApp()
    {
        register_cube(this);
        register_scene(this);
        register_lights(this);
    }
};

IMPLEMENT_MAIN(ExampleApp)
