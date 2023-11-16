#include "klotter/render/mesh.builder.h"

#include "sample.h"

#include <cmath>

using namespace klotter;

namespace examples
{

struct CubeSample : Sample
{
	World world;
	MeshInstancePtr cube;

	CubeSample(Renderer* renderer, Camera* camera)
	{
		camera->position.z = 0;
		camera->position.x = -3;
		const auto triangle
			= mesh::create_box(0.5f, 2.0f, 1.5f, false, colors::red_vermillion).to_mesh();
		auto material = renderer->make_basic_material();
		material->texture = renderer->assets.get_light_grid();
		auto geometry = compile_Mesh(triangle, material);

		cube = make_MeshInstance(geometry);
		world.meshes.emplace_back(cube);
	}

	float p = 0.0f;

	void on_render(klotter::Renderer* renderer, klotter::Camera* camera, float dt) override
	{
		// cube->rotation.x += time;
		p += dt;
		cube->position.z = std::cos(p * 2.1f) - 1.0f;
		cube->position.x = std::sin(p * 1.4f) * 0.6f;
		cube->position.y = std::sin(p * 0.4f) * 0.5f;

		cube->rotation.z = p;
		cube->rotation.y = -p / 4.0f;
		cube->rotation.x = std::sin(p * 0.6f);

		renderer->render(world, *camera);
	}

	void on_gui(klotter::Camera*) override
	{
	}
};

void register_cube(SampleApp* app)
{
	app->add_sample<CubeSample>("Cube");
}

}  //  namespace examples
