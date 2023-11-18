#include "klotter/render/mesh.builder.h"

#include "sample.h"

#include <cmath>

using namespace klotter;

namespace examples
{

struct SceneSample : Sample
{
	World world;

	MeshInstancePtr add_cube(
		klotter::Renderer* renderer,
		std::shared_ptr<klotter::Texture> texture,
		float x,
		float y,
		float z,
		bool invert,
		const glm::vec3& color
	)
	{
		// todo(gustav): take mesh as a argument, don't create a new mesh for each sub cube
		const auto triangle = mesh::create_box(x, y, z, invert, color).to_mesh();
		auto material = renderer->make_unlit_material();
		material->texture = texture;
		auto geometry = compile_Mesh(triangle, material);

		auto cube = make_MeshInstance(geometry);
		world.meshes.emplace_back(cube);

		return cube;
	}

	void add_mini_cube(
		klotter::Renderer* renderer,
		std::shared_ptr<klotter::Texture> texture,
		const glm::vec3& p,
		int index
	)
	{
		auto cube = add_cube(renderer, texture, 1.0f, 1.0f, 1.0f, false, colors::white);
		cube->position = p;

		const auto fi = [index](int i) -> float
		{
			return 5.0f * static_cast<float>((index + i) % 10);
		};
		cube->rotation.x = fi(1);
		cube->rotation.y = fi(3);
		cube->rotation.z = fi(5);
	}

	SceneSample(klotter::Renderer* renderer, klotter::Camera* camera)
	{
		camera->pitch = 15;
		camera->yaw = -50;

		auto t = renderer->assets.get_light_grid();
		// add world
		add_cube(renderer, t, 10.0f, 10.0f, 10.0f, true, colors::blue_sky);

		add_mini_cube(renderer, t, {1.5f, 2.0f, 2.5f}, 0);
		add_mini_cube(renderer, t, {1.5f, 0.2f, -1.5f}, 1);
		add_mini_cube(renderer, t, {2.4f, -0.4f, 3.5f}, 2);
		add_mini_cube(renderer, t, {1.3f, -2.0f, -2.5f}, 3);
		add_mini_cube(renderer, t, {-1.3f, 1.0f, 1.5f}, 4);
		add_mini_cube(renderer, t, {-1.7f, 3.0f, -7.5f}, 5);
		add_mini_cube(renderer, t, {-1.5f, -2.2f, 2.5f}, 6);
		add_mini_cube(renderer, t, {-3.8f, -2.0f, -2.3f}, 7);
	}

	void on_render(klotter::Renderer* renderer, klotter::Camera* camera, float) override
	{
		renderer->render(world, *camera);
	}

	void on_gui(klotter::Camera*) override
	{
	}
};

void register_scene(SampleApp* app)
{
	app->add_sample<SceneSample>("Scene");
}

}  //  namespace examples
