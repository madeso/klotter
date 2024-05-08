#include "klotter/render/geom.builder.h"

#include "sample.h"

#include <cmath>

using namespace klotter;

namespace examples
{

struct SceneSample : Sample
{
	World world;
	EffectStack effects;

	std::shared_ptr<CompiledGeom> make_unlit_cube(
		klotter::Renderer* renderer, float x, float y, float z, bool invert
	)
	{
		const auto triangles = geom::create_box(x, y, z, invert).to_geom();
		auto geom = compile_geom(triangles, renderer->unlit_geom_layout());
		return geom;
	}

	std::shared_ptr<MeshInstance> add_unlit_cube(
		klotter::Renderer* renderer,
		std::shared_ptr<klotter::Texture2d> texture,
		std::shared_ptr<CompiledGeom> geom,
		const glm::vec3& color
	)
	{
		auto material = renderer->make_unlit_material();
		material->color = color;
		material->texture = texture;

		auto cube = make_mesh_instance(geom, material);
		world.meshes.emplace_back(cube);

		return cube;
	}

	void add_unlit_mini_cube(
		klotter::Renderer* renderer,
		std::shared_ptr<CompiledGeom> geom,
		std::shared_ptr<klotter::Texture2d> texture,
		const glm::vec3& p,
		int index
	)
	{
		auto cube = add_unlit_cube(renderer, texture, geom, colors::white);
		cube->position = p;

		const auto fi = [index](int i)
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
		auto world_geom = make_unlit_cube(renderer, 10.0f, 10.0f, 10.0f, true);
		add_unlit_cube(renderer, t, world_geom, colors::blue_sky);

		auto mini = make_unlit_cube(renderer, 1.0f, 1.0f, 1.0f, false);
		add_unlit_mini_cube(renderer, mini, t, {1.5f, 2.0f, 2.5f}, 0);
		add_unlit_mini_cube(renderer, mini, t, {1.5f, 0.2f, -1.5f}, 1);
		add_unlit_mini_cube(renderer, mini, t, {2.4f, -0.4f, 3.5f}, 2);
		add_unlit_mini_cube(renderer, mini, t, {1.3f, -2.0f, -2.5f}, 3);
		add_unlit_mini_cube(renderer, mini, t, {-1.3f, 1.0f, 1.5f}, 4);
		add_unlit_mini_cube(renderer, mini, t, {-1.7f, 3.0f, -7.5f}, 5);
		add_unlit_mini_cube(renderer, mini, t, {-1.5f, -2.2f, 2.5f}, 6);
		add_unlit_mini_cube(renderer, mini, t, {-3.8f, -2.0f, -2.3f}, 7);
	}

	void on_render(
		const glm::ivec2& window_size, klotter::Renderer* renderer, klotter::Camera* camera, float
	) override
	{
		effects.render({&world, window_size, camera, renderer});
	}

	void on_gui(klotter::Camera*) override
	{
		// no ui needed
	}
};

void add_sample_scene(SampleApp* app)
{
	app->add_sample<SceneSample>("Scene");
}

}  //  namespace examples
