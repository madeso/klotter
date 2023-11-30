#include "klotter/render/geom.builder.h"
#include "klotter/str.h"

#include "sample.h"

#include <cmath>

using namespace klotter;

namespace examples
{

struct LightsSample : Sample
{
	World world;
	std::vector<std::shared_ptr<MeshInstance>> cubes;
	float anim = 0.0f;

	std::shared_ptr<CompiledGeom> create_cube_geom(
		float x, float y, float z, bool invert, CompiledGeomVertexAttributes layout
	)
	{
		const auto triangle = geom::create_box(x, y, z, invert, colors::white).to_geom();
		auto geom = compile_geom(triangle, layout);
		return geom;
	}

	std::shared_ptr<MeshInstance> add_cube(
		std::shared_ptr<CompiledGeom> geom, std::shared_ptr<Material> material
	)
	{
		auto cube = make_mesh_instance(geom, material);
		world.meshes.emplace_back(cube);
		return cube;
	}

	std::shared_ptr<klotter::DefaultMaterial> add_mini_cube(
		klotter::Renderer* renderer,
		std::shared_ptr<CompiledGeom> geom,
		std::shared_ptr<klotter::Texture> diffuse,
		std::shared_ptr<klotter::Texture> specular,
		const glm::vec3& p
	)
	{
		auto material = renderer->make_default_material();
		material->diffuse = diffuse;
		material->specular = specular;

		auto cube = add_cube(geom, material);
		cube->position = p;

		cubes.emplace_back(cube);

		return material;
	}

	std::shared_ptr<UnlitMaterial> light_material;

	LightsSample(Renderer* renderer, Camera* camera)
	{
		camera->pitch = 15;
		camera->yaw = -50;

		light_material = renderer->make_unlit_material();
		auto light_geom
			= create_cube_geom(0.25f, 0.25f, 0.25f, false, renderer->unlit_geom_layout());
		auto light = add_cube(light_geom, light_material);
		light->position.z = 0.5f;
		world.lights.point_light.position = light->position;

		auto mini = compile_geom(
			geom::create_uv_sphere(1.0f, 9, 9, false).write_obj("mini-sphere.obj").to_geom(),
			renderer->default_geom_layout()
		);
		auto mini2 = create_cube_geom(1.0f, 1.0f, 1.0f, false, renderer->default_geom_layout());
		auto t = renderer->assets.get_light_grid();
		auto s = renderer->assets.get_white();
		auto ct = renderer->assets.get_container_diffuse();
		auto cs = renderer->assets.get_container_specular();
		add_mini_cube(renderer, mini, ct, cs, {1.5f, 2.0f, 2.5f});
		add_mini_cube(renderer, mini2, ct, cs, {1.5f, 0.2f, -1.5f});
		add_mini_cube(renderer, mini, t, s, {2.4f, -0.4f, 3.5f});
		add_mini_cube(renderer, mini, t, s, {1.3f, -2.0f, -2.5f});
		add_mini_cube(renderer, mini, t, s, {-1.3f, 1.0f, 1.5f});
		auto mat = add_mini_cube(renderer, mini2, ct, cs, {-1.7f, 3.0f, -3.5f});
		add_mini_cube(renderer, mini, t, s, {-1.5f, -2.2f, 2.5f});
		add_mini_cube(renderer, mini, t, s, {-3.8f, -2.0f, -2.3f});
		mat->emissive = renderer->assets.get_matrix();
		mat->emissive_factor = 1.0f;
		apply_animation();
	}

	void apply_animation()
	{
		int index = 0;
		int cube_count = static_cast<int>(cubes.size() + 1);
		for (auto& cube: cubes)
		{
			const auto fi = [this, index, cube_count](int i) -> float
			{
				return anim + 5.0f * static_cast<float>((index + i) % cube_count);
			};
			cube->rotation.x = fi(1);
			cube->rotation.y = fi(3);
			cube->rotation.z = fi(5);
		}
	}

	void on_render(
		const glm::ivec2& window_size,
		klotter::Renderer* renderer,
		klotter::Camera* camera,
		float dt
	) override
	{
		anim += dt * 0.25f;
		apply_animation();
		renderer->render(window_size, world, *camera);
	}

	void on_gui(klotter::Camera* camera) override
	{
		ImGui::LabelText("pitch", "%s", (Str{} << camera->pitch).str().c_str());
		ImGui::LabelText("yaw", "%s", (Str{} << camera->yaw).str().c_str());

		const float speed = 0.1f;
		auto& pl = world.lights.point_light;
		const auto m = pl.min_range;
		if (ImGui::DragFloat("min", &pl.min_range, speed))
		{
			pl.min_range = std::max(0.0f, pl.min_range);
			const auto change = pl.min_range - m;
			pl.max_range += change;
		}
		if (ImGui::DragFloat("max", &pl.max_range, speed))
		{
			pl.max_range = std::max(pl.max_range, pl.min_range + 0.01f);
		}
		imgui_s_curve_editor("att", &pl.curve, true);
	}
};

void add_sample_lights(SampleApp* app)
{
	app->add_sample<LightsSample>("Lights");
}

}  //  namespace examples
