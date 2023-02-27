#include "klotter/klotter.h"

#include <cmath>

using namespace klotter;

constexpr auto fov = 75.0f;
constexpr auto near = 0.1f;
constexpr auto far = 5.0f;

struct CubeApp : App
{
    Camera camera;
    Scene scene;
    MeshInstancePtr cube;

    CubeApp()
        : camera{fov, near, far}
    {
        camera.position.z = 2;

        const auto left = 0.0f;
        const auto right = 2.0;
        const auto top = 2.0f;
        const auto bottom = 0.0f;
        
        const auto triangle = Mesh
        {
            {
                Vertex{{ 0.5f,  0.5f, 0.0f}, {right, top}},  // top right
                Vertex{{ 0.5f, -0.5f, 0.0f}, {right, bottom}},  // bottom right
                Vertex{{-0.5f, -0.5f, 0.0f}, {left, bottom}},  // bottom left
                Vertex{{-0.5f,  0.5f, 0.0f}, {left, top}}   // top left 
            },
            {
                Face{0, 1, 3},   // first triangle
                Face{1, 2, 3}    // second triangle
            }
        };
        auto material = std::make_shared<BasicMaterial>();
        material->color = {white, 1.0f};
        // material->texture = renderer.assets.get_dark_grid();
        auto geometry = compile_Mesh(triangle, material);
        
        cube = make_MeshInstance(geometry);
        scene.meshes.emplace_back(cube);
    }

    float p = 0.0f;
    void on_render(float dt) override
    {
        // cube->rotation.x += time;
        p += dt * 2.0f;
        cube->position.x = sin(p) * 0.6f;
        cube->rotation.z += dt;

        renderer.render(scene, camera);
    }
};

IMPLEMENT_MAIN(CubeApp)
