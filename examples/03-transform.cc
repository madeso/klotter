#include "klotter/render/geom.builder.h"

#include "sample.h"

#include <cmath>

using namespace klotter;

namespace examples
{

constexpr int display_count = 3;
constexpr float cube_size = 1.0f;

struct TransformSample : Sample
{
	World world;

	std::shared_ptr<MeshInstance> center;
	std::vector<std::shared_ptr<MeshInstance>> before;
	std::vector<std::shared_ptr<MeshInstance>> after;

	bool show_x = true;
	bool show_y = true;
	bool show_z = true;

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
		std::shared_ptr<klotter::Texture> texture,
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

	TransformSample(klotter::Renderer* renderer, klotter::Camera* camera)
	{
		camera->pitch = 15;
		camera->yaw = -50;

		auto t = renderer->assets.get_light_grid();

		// add world
		// auto world_geom = make_unlit_cube(renderer, 10.0f, 10.0f, 10.0f, true);
		// add_unlit_cube(renderer, t, world_geom, colors::blue_sky);

		auto mini = make_unlit_cube(renderer, cube_size, cube_size, cube_size, false);

		for (int i = 0; i < display_count; i += 1)
		{
			before.emplace_back(add_unlit_cube(renderer, t, mini, colors::white));
		}
		center = add_unlit_cube(renderer, t, mini, colors::white);
		for (int i = 0; i < display_count; i += 1)
		{
			after.emplace_back(add_unlit_cube(renderer, t, mini, colors::white));
		}
	}

	void on_render(
		const glm::ivec2& window_size, klotter::Renderer* renderer, klotter::Camera* camera, float
	) override
	{
		const auto length = 5.0f;
		const auto axis = center->get_local_axis();
		const auto offset = axis.z * (cube_size * 1.1f);
		const auto cp = center->position;

		if (show_x)
		{
			renderer->debug.add_line(
				cp,
				cp + axis.x * length,
				klotter::colors::orange,
				klotter::LineStyle::dashed_when_hidden
			);
		}

		if (show_y)
		{
			renderer->debug.add_line(
				cp,
				cp + axis.y * length,
				klotter::colors::orange,
				klotter::LineStyle::dashed_when_hidden
			);
		}

		if (show_z)
		{
			renderer->debug.add_line(
				cp,
				cp + axis.z * length,
				klotter::colors::orange,
				klotter::LineStyle::dashed_when_hidden
			);
		}

		for (int i = 0; i < display_count; i += 1)
		{
			const auto si = static_cast<std::size_t>(i);
			before[si]->position = cp - offset * static_cast<float>(i + 1);
			after[si]->position = cp + offset * static_cast<float>(i + 1);

			before[si]->rotation = after[si]->rotation = center->rotation;
		}

		renderer->render(window_size, world, *camera);
	}

	void on_gui(klotter::Camera*) override
	{
		ImGui::DragFloat3("Position", glm::value_ptr(center->position), 0.1f);
		ImGui::Checkbox("X", &show_x);
		ImGui::SameLine();
		ImGui::Checkbox("Y", &show_y);
		ImGui::SameLine();
		ImGui::Checkbox("Z", &show_z);

		const auto min = -180.0f;
		const auto max = 180.0f;
		ImGui::SliderAngle("yaw", &center->rotation.x, min, max);
		ImGui::SliderAngle("pitch", &center->rotation.y, min, max);
		ImGui::SliderAngle("roll", &center->rotation.z, min, max);
	}
};

void add_sample_transform(SampleApp* app)
{
	app->add_sample<TransformSample>("Transform");
}

}  //  namespace examples
