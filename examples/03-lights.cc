#include "klotter/render/geom.builder.h"
#include "klotter/str.h"
#include "klotter/cint.h"

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
	bool follow_player = true;

	LightsSample(Renderer* renderer, Camera* camera)
	{
		camera->pitch = 15;
		camera->yaw = -50;

		light_material = renderer->make_unlit_material();
		auto light_geom
			= create_cube_geom(0.25f, 0.25f, 0.25f, false, renderer->unlit_geom_layout());
		auto light = add_cube(light_geom, light_material);
		light->position.z = 0.5f;

		// ambient
		world.lights.ambient = 0.1f;

		// directional
		{
			world.lights.directional_lights.emplace_back();
			auto& dili = world.lights.directional_lights[0];
			dili.direction = {-1.0f, -1.0f, -1.0f};
			dili.direction = glm::normalize(dili.direction);
			dili.diffuse = 0.1f;
			dili.specular = dili.diffuse;
			dili.color = colors::red_vermillion;
		}

		// point light
		world.lights.point_lights.emplace_back();
		world.lights.point_lights[0].position = light->position;
		world.lights.point_lights[0].diffuse = 0.4f;
		world.lights.point_lights[0].specular = world.lights.point_lights[0].diffuse;

		// frustum
		{
			world.lights.frustum_lights.emplace_back();
			world.lights.frustum_lights[0].diffuse = 0.4f;
			world.lights.frustum_lights[0].specular = world.lights.frustum_lights[0].diffuse;
		}

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

		{
			constexpr auto PLANE_SIZE = 100.0f;
			auto plane_geom = compile_geom(
				geom::create_plane(PLANE_SIZE, PLANE_SIZE, false).to_geom(),
				renderer->default_geom_layout()
			);

			auto material = renderer->make_default_material();
			material->diffuse = renderer->assets.get_light_grid();
			material->specular = renderer->assets.get_white();

			auto plane = add_cube(plane_geom, material);
			plane->position = {0.0f, -3.0f, 0.0f};
		}

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
		if (follow_player)
		{
			auto& fl = world.lights.frustum_lights[0];
			fl.position = camera->position;
			fl.yaw = camera->yaw;
			fl.pitch = camera->pitch;
		}
		anim += dt * 0.25f;
		apply_animation();
		renderer->render(window_size, world, *camera);
	}

	static void min_max(float* min_range, float* max_range)
	{
		const float RANGE_SPEED = 0.1f;

		const auto m = *min_range;
		if (ImGui::DragFloat("min", min_range, RANGE_SPEED))
		{
			*min_range = std::max(0.0f, *min_range);
			const auto change = *min_range - m;
			*max_range += change;
		}
		if (ImGui::DragFloat("max", max_range, RANGE_SPEED))
		{
			*max_range = std::max(*max_range, *min_range + 0.01f);
		}
	}

	void on_gui(klotter::Camera* camera) override
	{
		const float FAC_SPEED = 0.01f;

		ImGui::DragFloat3("position", glm::value_ptr(camera->position));
		ImGui::LabelText("pitch", "%s", (Str{} << camera->pitch).str().c_str());
		ImGui::LabelText("yaw", "%s", (Str{} << camera->yaw).str().c_str());

		ImGui::DragFloat("Ambient", &world.lights.ambient, FAC_SPEED, 0.0f, 1.0f);


		for (int dir_light_index = 0;
			 dir_light_index < Csizet_to_int(world.lights.directional_lights.size());
			 dir_light_index += 1)
		{
			ImGui::PushID(dir_light_index);
			auto& dl = world.lights.directional_lights[Cint_to_sizet(dir_light_index)];
			if (ImGui::DragFloat("Directional", &dl.diffuse, FAC_SPEED, 0.0f, 1.0f))
			{
				dl.specular = dl.diffuse;
			}
			ImGui::PopID();
		}

		for (int point_light_index = 0;
			 point_light_index < Csizet_to_int(world.lights.point_lights.size());
			 point_light_index += 1)
		{
			auto& pl = world.lights.point_lights[Cint_to_sizet(point_light_index)];
			ImGui::PushID(point_light_index);

			if (ImGui::DragFloat("Point", &pl.diffuse, FAC_SPEED, 0.0f, 1.0f))
			{
				pl.specular = pl.diffuse;
			}
			min_max(&pl.min_range, &pl.max_range);
			imgui_s_curve_editor("att", &pl.curve, true);
			ImGui::PopID();
		}

		ImGui::PushID("frustum lights");
		{
			ImGui::Checkbox("Follow player", &follow_player);
			auto& fl = world.lights.frustum_lights[0];

			if (follow_player == false)
			{
				ImGui::DragFloat3("position", glm::value_ptr(fl.position));
				ImGui::DragFloat("yaw", &fl.yaw);
				ImGui::DragFloat("pitch", &fl.pitch);
			}
			else
			{
				ImGui::Text(
					"position (%f %f %f)",
					static_cast<double>(fl.position.x),
					static_cast<double>(fl.position.y),
					static_cast<double>(fl.position.z)
				);
				ImGui::Text(
					"ypr (%f %f)", static_cast<double>(fl.pitch), static_cast<double>(fl.yaw)
				);
			}
			if (ImGui::DragFloat("Frustum", &fl.diffuse, FAC_SPEED, 0.0f, 1.0f))
			{
				fl.specular = fl.diffuse;
			}
			ImGui::DragFloat("fov", &fl.fov, 0.1f);
			ImGui::DragFloat("aspect", &fl.aspect, 0.001f);
			min_max(&fl.min_range, &fl.max_range);
			imgui_s_curve_editor("att", &fl.curve, true);
		}
		ImGui::PopID();
	}
};

void add_sample_lights(SampleApp* app)
{
	app->add_sample<LightsSample>("Lights");
}

}  //  namespace examples
