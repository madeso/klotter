#pragma once


#include "klotter/render/render.h"
#include "klotter/dependency_sdl.h"


namespace klotter
{

struct App
{
    Renderer renderer;

    virtual ~App() = default;

    virtual void on_render(float dt) = 0;
};

using MakeAppFunction = std::function<std::unique_ptr<App>()>;
int run_main(MakeAppFunction make_app);

const glm::ivec2 get_window_size();

}


#define IMPLEMENT_MAIN(APP) int main(int, char**)\
{ return klotter::run_main([](){ return std::make_unique<APP>();}); }

