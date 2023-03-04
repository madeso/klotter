#include "klotter/klotter.h"
#include "klotter/render/mesh.builder.h"

#include <cmath>

using namespace klotter;

std::shared_ptr<Scene> make_cube(Renderer* renderer);
std::shared_ptr<Scene> make_scene(Renderer* renderer);

enum class SceneType { cube, scene };

struct ExampleApp : App
{
    SceneType scene_type = SceneType::cube;

    std::shared_ptr<Scene> cube_scene;
    std::shared_ptr<Scene> scene_scene;

    void set_scene(SceneType new_scene)
    {
        scene_type = new_scene;
        switch (scene_type)
        {
#define CASE(x) case SceneType::x: if(x##_scene == nullptr) { x##_scene = make_##x(&renderer); } scene = x##_scene; break
            CASE(cube);
            CASE(scene);
#undef CASE
        default:
            assert(false && "unhandled type");
            break;
        }
    }

    ExampleApp()
    {
        set_scene(SceneType::cube);
    }

    void on_gui() override
    {
        // ImGui::ShowDemoWindow();

        ImGui::Begin("Scene switcher");
        if (ImGui::Button("Cube")) { set_scene(SceneType::cube); }
        if (ImGui::Button("Scene")) { set_scene(SceneType::scene); }
        ImGui::End();
    }
};

IMPLEMENT_MAIN(ExampleApp)
