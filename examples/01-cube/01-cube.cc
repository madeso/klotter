#include "klotter/klotter.h"
#include "klotter/render/mesh.builder.h"

#include <cmath>

using namespace klotter;

struct CubeApp : App
{
    Camera camera;
    Scene scene;
    MeshInstancePtr cube;

    CubeApp()
    {
        camera.position.z = 2;
        const auto triangle = mesh::CreateBox(1.0f, 1.0f, 1.0f, false).asMesh();
        auto material = std::make_shared<BasicMaterial>();
        // material->color = {white, 1.0f};
        material->texture = renderer.assets.get_dark_grid();
        auto geometry = compile_Mesh(triangle, material);
        
        cube = make_MeshInstance(geometry);
        scene.meshes.emplace_back(cube);
    }

    float p = 0.0f;
    void on_render(float dt) override
    {
        // cube->rotation.x += time;
        p += dt * 2.0f;
        cube->position.z = std::cos(p) - 1.0f;
        cube->position.x = std::sin(p) * 0.6f;
        cube->rotation.z += dt;

        renderer.render(scene, camera);
    }
};

IMPLEMENT_MAIN(CubeApp)
