#include "klotter/klotter.h"

using namespace klotter;

constexpr auto fov = 75.0f;
constexpr auto near = 0.1f;
constexpr auto far = 5.0f;

struct CubeApp : App
{
    Camera camera;
    Scene scene;
    MeshPtr cube;

    CubeApp()
        : camera{fov, near, far}
    {
        camera.position.z = 2;

        const auto boxWidth = 1;
        const auto boxHeight = 1;
        const auto boxDepth = 1;
        auto geometry = make_BoxGeometry(boxWidth, boxHeight, boxDepth);
        auto material = make_MeshBasicMaterial();
        
        cube = make_Mesh(geometry, material);
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
