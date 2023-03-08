#pragma once

#include "klotter/klotter.h"

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

struct SceneApp: App
{
    std::vector<SceneType> types;
    std::size_t scene_index;
    std::shared_ptr<Scene> selected_scene;
    std::shared_ptr<Scene> active_scene;

    void select_scene(std::size_t new_scene);

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

    void on_frame() override;
    void on_gui() override;
    void on_render(float) override;
};


}