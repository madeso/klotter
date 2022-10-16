#include "three/three.h"

using namespace three;

constexpr auto fov = 75.0f;
constexpr auto near = 0.1f;
constexpr auto far = 5.0f

struct CubeApp : App
{
    Camera camera;
    Scene scene;
    MeshPtr cube;

    CubeApp()
        : camera{fov, near, far};
    {
        camera.position.z = 2;

            const auto boxWidth = 1;
            const auto boxHeight = 1;
            const auto boxDepth = 1;
            auto geometry = make_BoxGeometry(boxWidth, boxHeight, boxDepth);
            auto material = make_MeshBasicMaterial();
            material->color = color_from_hex(0x44aa88);
            
            cube = make_Mesh(geometry, material);
            scene.add(cube);
    }

    void on_render(float time) override
    {
        cube->rotation.x = time;
        cube->rotation.y = time;

        render(scene, camera);
    }


};
