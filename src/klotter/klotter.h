#pragma once

#include <memory>

#include "imgui.h"

#include "klotter/render/render.h"
#include "klotter/dependency_sdl.h"


namespace klotter
{

struct Scene
{
    virtual ~Scene() = default;

    Camera camera;

    virtual void on_render(float dt) = 0;
    virtual void on_gui() = 0;
};

struct SceneType
{
    std::string name;
    std::function<std::shared_ptr<Scene>(Renderer*)> create;
};

struct App
{
    Renderer renderer;
    std::vector<SceneType> types;

    template<typename T>
    void add_type(const std::string& name)
    {
        types.emplace_back(
            SceneType{name,
                [](Renderer* r) -> std::shared_ptr<Scene>
                { return std::make_shared<T>(r); }
            }
        );
    }

    virtual ~App() = default;
};

using MakeAppFunction = std::function<std::unique_ptr<App>()>;
int run_main(MakeAppFunction make_app);

}


#define IMPLEMENT_MAIN(APP) int main(int, char**)\
{ return klotter::run_main([](){ return std::make_unique<APP>();}); }

