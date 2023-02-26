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

        auto triangle = Mesh{};
        triangle.faces.push_back({
            glm::vec3{-0.5f, -0.5f, 0.0f},
            glm::vec3{ 0.5f, -0.5f, 0.0f},
            glm::vec3{ 0.0f,  0.5f, 0.0f}
        });

        auto material = make_BasicMaterial();
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
