#include "klotter/klotter.h"
#include "klotter/render/mesh.builder.h"

#include <cmath>

using namespace klotter;

struct CubeApp : App
{
    Scene scene;
    MeshInstancePtr cube;

    CubeApp()
    {
        camera.position.z = 2;
        const auto triangle = mesh::create_box(0.5f, 2.0f, 1.5f, false, colors::red_vermillion).to_mesh();
        auto material = std::make_shared<BasicMaterial>();
        // material->color = {white, 1.0f};
        material->texture = renderer.assets.get_light_grid();
        auto geometry = compile_Mesh(triangle, material);
        
        cube = make_MeshInstance(geometry);
        scene.meshes.emplace_back(cube);
    }

    float p = 0.0f;
    void on_render(float dt) override
    {
        // cube->rotation.x += time;
        p += dt;
        cube->position.z = std::cos(p * 2.1f) - 1.0f;
        cube->position.x = std::sin(p * 1.4f) * 0.6f;
        cube->position.y = std::sin(p * 0.4f) * 0.5f;

        cube->rotation.z = p;
        cube->rotation.y = -p / 4.0f;
        cube->rotation.x = std::sin(p * 0.6f);

        renderer.render(scene, camera);
    }
};

IMPLEMENT_MAIN(CubeApp)
