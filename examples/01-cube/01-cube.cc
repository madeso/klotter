#include "klotter/klotter.h"

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

        const auto triangle = Mesh
        {
            {
                Vertex{{ 0.5f,  0.5f, 0.0f}, red_vermillion},  // top right
                Vertex{{ 0.5f, -0.5f, 0.0f}, blue},  // bottom right
                Vertex{{-0.5f, -0.5f, 0.0f}, green_bluish},  // bottom left
                Vertex{{-0.5f,  0.5f, 0.0f}, orange}   // top left 
            },
            {
                Face{0, 1, 3},   // first triangle
                Face{1, 2, 3}    // second triangle
            }
        };

        auto material = std::make_shared<BasicMaterial>();
        material->color = {white, 1.0f};
        auto geometry = compile_Mesh(triangle, material);
        
        cube = make_MeshInstance(geometry);
        scene.meshes.emplace_back(cube);
    }

    void on_render(float time) override
    {
        cube->rotation.x = time;
        cube->rotation.y = time;

        render(scene, camera);
    }
};

IMPLEMENT_MAIN(CubeApp)
