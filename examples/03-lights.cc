#include "klotter/render/mesh.builder.h"
#include "klotter/str.h"

#include "scene.h"

#include <cmath>

using namespace klotter;

namespace examples
{

struct LightsScene : Scene
{
	Renderer* renderer;
	Camera* camera;

	World world;
	std::vector<MeshInstancePtr> cubes;
	float anim = 0.0f;

	MeshInstancePtr add_cube(
		float x, float y, float z, bool invert, MaterialPtr material, const glm::vec3& color
	)
	{
		const auto triangle = mesh::create_box(x, y, z, invert, color).to_mesh();
		auto geometry = compile_Mesh(triangle, material);

		auto cube = make_MeshInstance(geometry);
		world.meshes.emplace_back(cube);

		return cube;
	}

	void add_mini_cube(const glm::vec3& p)
	{
		auto material = std::make_shared<LightMaterial>();
		material->texture = renderer->assets.get_light_grid();

		auto cube = add_cube(1.0f, 1.0f, 1.0f, false, material, colors::white);
		cube->position = p;

		cubes.emplace_back(cube);
	}

	std::shared_ptr<BasicMaterial> light_material;

	LightsScene(Renderer* r, Camera* c)
		: renderer(r)
		, camera(c)
	{
		camera->pitch = 15;
		camera->yaw = -50;

		light_material = std::make_shared<BasicMaterial>();
		auto light = add_cube(0.25f, 0.25f, 0.25f, false, light_material, colors::white);
		light->position.z = 0.5f;

		add_mini_cube({1.5f, 2.0f, 2.5f});
		add_mini_cube({1.5f, 0.2f, -1.5f});
		add_mini_cube({2.4f, -0.4f, 3.5f});
		add_mini_cube({1.3f, -2.0f, -2.5f});
		add_mini_cube({-1.3f, 1.0f, 1.5f});
		add_mini_cube({-1.7f, 3.0f, -7.5f});
		add_mini_cube({-1.5f, -2.2f, 2.5f});
		add_mini_cube({-3.8f, -2.0f, -2.3f});
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

	void on_render(float dt) override
	{
		anim += dt * 0.25f;
		apply_animation();
		renderer->render(world, *camera);
	}

	void on_gui() override
	{
		ImGui::LabelText("pitch", "%s", (Str{} << camera->pitch).str().c_str());
		ImGui::LabelText("yaw", "%s", (Str{} << camera->yaw).str().c_str());
	}
};

void register_lights(SceneApp* app)
{
	app->add_type<LightsScene>("Lights");
}

}  //  namespace examples
