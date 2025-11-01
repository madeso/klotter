#include <cmath>

#include "klotter/klotter.h"
#include "klotter/str.h"
#include "klotter/cint.h"
#include "klotter/imgui.theme.h"
#include "klotter/im_colors.h"

#include "klotter/render/geom.builder.h"
#include "klotter/render/geom.h"
#include "klotter/render/ui.h"
#include "klotter/render/postproc.internal.h" // todo(Gustav): only needed for visualizing the shadow depth buffer, improve design and remove

using namespace klotter;

namespace examples
{

static void imgui_label(const char* const label, const std::string& text)
{
	ImGui::LabelText(label, "%s", text.c_str());
}

static void imgui_color(const char* const label, klotter::Color* color)
{
	ImGui::ColorEdit3(label, &color->r);
}

static bool imgui_group_button(const char* label, bool is_first, bool is_selected)
{
	const auto& color = is_selected ? klotter::imgui::red : klotter::imgui::gray;
	ImGui::PushStyleColor(ImGuiCol_Button, color[6]);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color[7]);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, color[5]);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.0f, 0.0f});
	if (is_first == false)
	{
		ImGui::SameLine();
	}

	const auto result = ImGui::Button(label);

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);

	return result;
}

constexpr float FAC_SPEED = 0.01f;
constexpr float MAX_LIGHT = 100.0f;

struct LightsSample : klotter::App
{
	World world;
	EffectStack effects;
	std::shared_ptr<FactorEffect> pp_invert;
	std::shared_ptr<FactorEffect> pp_grayscale;
	std::shared_ptr<FactorEffect> pp_damage;
	std::shared_ptr<FactorEffect> pp_blur;

	std::vector<std::shared_ptr<MeshInstance>> cubes;
	float anim = 0.0f;

	static klotter::RenderSettings get_render_settings()
	{
		// use default
		return klotter::RenderSettings{};
	}

	std::shared_ptr<CompiledGeom> create_cube_geom(
		DEBUG_LABEL_ARG_MANY
		float x, float y, float z, klotter::geom::NormalsFacing normals_facing, CompiledGeomVertexAttributes layout
	)
	{
		const auto geom = geom::create_box(x, y, z, normals_facing, colors::white).to_geom();
		auto compiled_geom = compile_geom(SEND_DEBUG_LABEL_MANY(debug_label) geom, layout);
		return compiled_geom;
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
		std::shared_ptr<klotter::Texture2d> diffuse,
		std::shared_ptr<klotter::Texture2d> specular,
		const glm::vec3& p
	)
	{
		auto material = renderer->make_default_material();
		material->diffuse = diffuse;
		material->specular = specular;

		auto cube = add_cube(geom, material);
		cube->world_position = p;

		cubes.emplace_back(cube);

		return material;
	}

	std::shared_ptr<UnlitMaterial> light_material;
	bool follow_player = true;
	Skybox skybox;

	LightsSample(Renderer* renderer)
		: pp_invert(renderer->make_invert_effect())
		, pp_grayscale(renderer->make_grayscale_effect())
		, pp_damage(renderer->make_damage_effect())
		, pp_blur(renderer->make_blur_effect())
		, light_material(renderer->make_unlit_material())
		, skybox(renderer->make_skybox(renderer->assets.get_skybox()))
	{
		camera.pitch = 15;
		camera.yaw = -50;

		effects.effects.emplace_back(pp_invert);
		effects.effects.emplace_back(pp_grayscale);
		effects.effects.emplace_back(pp_blur);
		effects.effects.emplace_back(pp_damage);

		auto light_geom
			= create_cube_geom(USE_DEBUG_LABEL_MANY("light") 0.25f, 0.25f, 0.25f, geom::NormalsFacing::Out, renderer->unlit_geom_layout());
		auto light = add_cube(light_geom, light_material);
		light->world_position.z = 0.5f;

		// skybox
		world.skybox = skybox;

		// ambient
		world.lights.ambient_strength = 0.007f;


		// directional
		{
			world.lights.directional_lights.emplace_back();
			auto& dili = world.lights.directional_lights[0];
			dili.pitch = -45.0f;
			dili.yaw = 45.0f;
			dili.diffuse_strength = 0.007f;
			dili.specular_strength = dili.diffuse_strength;
			dili.color = colors::red_vermillion;
		}

		// point light
		world.lights.point_lights.emplace_back();
		world.lights.point_lights[0].position = light->world_position;
		world.lights.point_lights[0].diffuse_strength = 0.133f;
		world.lights.point_lights[0].specular_strength = world.lights.point_lights[0].diffuse_strength;

		// frustum
		{
			world.lights.frustum_lights.emplace_back();
			auto& fl = world.lights.frustum_lights[0];
			fl.diffuse_strength = 0.40f;
			fl.min_range = 10.0f;
			fl.max_range = 40.0f;
			fl.fov = 20;
			fl.specular_strength = fl.diffuse_strength;
			fl.cookie = renderer->assets.get_cookie();
		}

		auto mini = compile_geom(
			USE_DEBUG_LABEL_MANY("sphere")
			geom::create_uv_sphere(1.0f, 9, 9, geom::NormalsFacing::Out).write_obj("mini-sphere.obj").to_geom(),
			renderer->default_geom_layout()
		);
		auto mini2 = create_cube_geom(USE_DEBUG_LABEL_MANY("mini2") 1.0f, 1.0f, 1.0f, geom::NormalsFacing::Out, renderer->default_geom_layout());
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

		// glasses
		{
			const auto triangle
				= geom::create_xy_plane(1.0f, 1.0f, geom::SideCount::two_sided).to_geom();
			auto geom = compile_geom(USE_DEBUG_LABEL_MANY("glass") triangle, renderer->default_geom_layout());

			auto glass_mat = renderer->make_default_material();
			glass_mat->diffuse = renderer->assets.get_glass();
			glass_mat->alpha = 0.3f;

			auto glass = add_cube(geom, glass_mat);
			glass->world_position = {0.0f, 0.0f, 1.0f};

			glass = add_cube(geom, glass_mat);
			glass->world_position = {0.0f, 0.0f, -1.0f};
		}

		// billboards
		{
			const auto billboard_g
				= geom::create_xy_plane(1.0f, 1.0f, geom::SideCount::two_sided).to_geom();
			auto billboard_cg = compile_geom(USE_DEBUG_LABEL_MANY("billboard") billboard_g, renderer->default_geom_layout());

			auto grass_mat = renderer->make_default_material();
			grass_mat->diffuse = renderer->assets.get_grass();
			grass_mat->specular_color = colors::black;

			auto glass = add_cube(billboard_cg, grass_mat);
			glass->world_position = {1.0f, 0.0f, 0.0f};
			glass->billboarding = Billboarding::axial_y;

			glass = add_cube(billboard_cg, grass_mat);
			glass->world_position = {-1.0f, 0.0f, -1.0f};
			glass->billboarding = Billboarding::axial_y_fast;
		}

		{
			constexpr auto PLANE_SIZE = 100.0f;
			auto plane_geom = compile_geom(
				USE_DEBUG_LABEL_MANY("plane")
				geom::create_xz_plane(PLANE_SIZE, PLANE_SIZE, false).to_geom(),
				renderer->default_geom_layout()
			);

			auto material = renderer->make_default_material();
			material->diffuse = renderer->assets.get_light_grid();
			material->specular = renderer->assets.get_white();

			auto plane = add_cube(plane_geom, material);
			plane->world_position = {0.0f, -3.0f, 0.0f};
		}

		// instances
		{
			constexpr std::size_t instance_count = 50;
			constexpr float cube_size = 0.75f;
			auto instances_geom = compile_geom_with_transform_instance(
				USE_DEBUG_LABEL_MANY("instanced box")
				geom::create_box(cube_size, cube_size, cube_size, geom::NormalsFacing::Out, colors::white).to_geom(),
				renderer->default_geom_layout(),
				instance_count
			);
			auto material = renderer->make_default_material();
			material->diffuse = renderer->assets.get_glass();

			auto instances = make_mesh_instance(instances_geom, material);

			const auto mk = [](const glm::vec3& pos, const glm::vec3& rot) {
				const auto translation = glm::translate(glm::mat4(1.0f), pos);
				const auto rotation = glm::yawPitchRoll(rot.x, rot.y, rot.z);
				return translation * rotation;
			};

			for(int i=0; i<10; i+=1)
			{
				for(int j=0; j<10; j+=1)
				{
					instances->world_from_locals.emplace_back(mk({float_from_int(i), float_from_int(j), -4}, {float_from_int(i)/10.0f * 90.0f, float_from_int(j)/10.0f * 90.0f, 0}));
				}
			}

			world.instances.emplace_back(instances);
		}

		apply_animation();
	}

	void apply_animation()
	{
		int index = 0;
		auto cube_count = static_cast<int>(cubes.size() + 1);
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

	int selected_instance_index = 2;
	glm::ivec2 last_window_size;
	glm::vec2 projected_target;

	std::vector<SCurveGuiState> point_light_curves;
	SCurveGuiState frustum_light_curve = SCurveGuiState::light_curve();
	ImguiShaderCache imgui_shader_cache;

	void on_render(
		const glm::ivec2& window_size,
		klotter::Renderer* renderer,
		float dt
	) override
	{
		effects.update(dt);
		auto& fl = world.lights.frustum_lights[0];
		if (follow_player)
		{
			fl.position = camera.position;
			fl.yaw = camera.yaw;
			fl.pitch = camera.pitch;
		}
		if (is_valid_instance_index(selected_instance_index))
		{
			const auto target = world.meshes[sizet_from_int(selected_instance_index)]->world_position;
			last_window_size = window_size;
			projected_target = screen_from_world(compile(camera, window_size), target, window_size);
			renderer->debug.add_line(glm::vec3{0, 0, 0}, target, klotter::colors::orange);
		}
		anim += dt * 0.25f;
		apply_animation();
		effects.render({&world, window_size, &camera, renderer});
	}

	void on_frame(Renderer*) override
	{
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

	bool is_valid_instance_index(int index)
	{
		return index >= 0 && sizet_from_int(index) < world.meshes.size();
	}

	void gui_ambient_light()
	{
		ImGui::DragFloat("Ambient strength", &world.lights.ambient_strength, FAC_SPEED, 0.0f, 1.0f);
		imgui_color("Ambient color", &world.lights.ambient_color);
	}

	void gui_direction_light(DirectionalLight& dl)
	{
		if (ImGui::DragFloat("Strength", &dl.diffuse_strength, FAC_SPEED, 0.0f, MAX_LIGHT))
		{
			dl.specular_strength = dl.diffuse_strength;
		}
		ImGui::DragFloat("Pitch", &dl.pitch, FAC_SPEED);
		ImGui::DragFloat("Yaw", &dl.yaw, FAC_SPEED);
		imgui_color("Color", &dl.color);
	}

	void gui_point_light(const bool is_first_frame, PointLight& pl, SCurveGuiState& ui_curve)
	{
		if (ImGui::DragFloat("Strength", &pl.diffuse_strength, FAC_SPEED, 0.0f, MAX_LIGHT))
		{
			pl.specular_strength = pl.diffuse_strength;
		}
		imgui_color("Color", &pl.color);
		min_max(&pl.min_range, &pl.max_range);
		imgui_s_curve_editor("att", &pl.curve, &ui_curve, FlipX::yes, {}, is_first_frame);
	}

	void gui_all_direction_lights()
	{
		ImGui::DragFloat("Shadow size", &world.lights.shadow_size, FAC_SPEED);
		ImGui::DragFloat3("Shadow offset", glm::value_ptr(world.lights.shadow_offset), FAC_SPEED);
		if (world.lights.directional_lights.empty() == false)
		{
			// todo(Gustav): this is clumsy...
			auto render_world = effects.render_world_ref.lock();
			if (render_world && render_world->shadow_buffer)
			{
				imgui_image("Shadow buffer", *render_world->shadow_buffer, &imgui_shader_cache, ImageShader::DepthOrtho);
			}
		}
		for (int dir_light_index = 0;
		     dir_light_index < int_from_sizet(world.lights.directional_lights.size());
		     dir_light_index += 1)
		{
			ImGui::PushID(dir_light_index);
			gui_direction_light(world.lights.directional_lights[sizet_from_int(dir_light_index)]);
			ImGui::PopID();
		}
	}

	void gui_all_point_lights()
	{
		const auto old_point_size = point_light_curves.size();
		point_light_curves.resize(world.lights.point_lights.size(), SCurveGuiState::light_curve());
		for (int point_light_index = 0;
		     point_light_index < int_from_sizet(world.lights.point_lights.size());
		     point_light_index += 1)
		{
			const auto is_first_frame = sizet_from_int(point_light_index) >= old_point_size;

			ImGui::PushID(point_light_index);
			gui_point_light(
				is_first_frame,
				world.lights.point_lights[sizet_from_int(point_light_index)],
				point_light_curves[sizet_from_int(point_light_index)]
			);
			ImGui::PopID();
		}
	}

	void gui_all_frustum_lights()
	{
		static bool is_first_frustum_frame = true;
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
		if (ImGui::DragFloat("Frustum strength", &fl.diffuse_strength, FAC_SPEED, 0.0f, MAX_LIGHT))
		{
			fl.specular_strength = fl.diffuse_strength;
		}
		imgui_color("Color", &fl.color);
		ImGui::DragFloat("fov", &fl.fov, 0.1f);
		ImGui::DragFloat("aspect", &fl.aspect, 0.001f);
		min_max(&fl.min_range, &fl.max_range);
		imgui_s_curve_editor("att", &fl.curve, &frustum_light_curve, FlipX::yes, {}, is_first_frustum_frame);
		is_first_frustum_frame = false;
	}

	void gui_effects_factors()
	{
		{
			auto factor = pp_invert->get_factor();
			if (ImGui::SliderFloat("invert", &factor, 0.0f, 1.0f))
			{
				pp_invert->set_factor(factor);
			}
		}
		{
			auto factor = pp_grayscale->get_factor();
			if (ImGui::SliderFloat("grayscale", &factor, 0.0f, 1.0f))
			{
				pp_grayscale->set_factor(factor);
			}
		}
		{
			auto factor = pp_blur->get_factor();
			if (ImGui::SliderFloat("blur", &factor, 0.0f, 1.0f))
			{
				pp_blur->set_factor(factor);
			}
		}
		{
			auto factor = pp_damage->get_factor();
			if (ImGui::SliderFloat("damage", &factor, 0.0f, 1.0f))
			{
				pp_damage->set_factor(factor);
			}
		}
	}

	void gui_msaa(klotter::Renderer* renderer)
	{
		struct MSaaSetting
		{
			const char* label;
			int value;

			constexpr MSaaSetting(const char* l, int v)
				: label(l)
				  , value(v)
			{}
		};

		constexpr std::size_t all_count = 4;
		constexpr std::array<MSaaSetting, all_count> all_settings
			= {MSaaSetting{"No", 0}, MSaaSetting{"x2", 2}, MSaaSetting{"x4", 4}, MSaaSetting{"x8", 8}};

		for (std::size_t sett_index=0; sett_index < all_count; sett_index +=1)
		{
			const auto sett = all_settings[sett_index];

			if (imgui_group_button(sett.label, sett_index == 0, renderer->settings.msaa == sett.value))
			{
				renderer->settings.msaa = sett.value;
			}
		}
		ImGui::SameLine();
		ImGui::Text("MSAA");
	}

	void gui_outline_toggle()
	{
		ImGui::SliderInt("Index", &selected_instance_index, 0, int_from_sizet(world.meshes.size()) - 1);
		if (is_valid_instance_index(selected_instance_index))
		{
			auto& inst = world.meshes[sizet_from_int(selected_instance_index)];
			bool check = inst->outline.has_value();
			if (ImGui::Checkbox("outline?", &check))
			{
				if (inst->outline)
				{
					inst->outline = std::nullopt;
				}
				else
				{
					inst->outline = colors::blue_sky;
				}
			}
		}
	}

	void gui_skybox()
	{
		bool has_skybox = world.skybox.has_value();
		if (ImGui::Checkbox("Skybox", &has_skybox))
		{
			if (has_skybox)
			{
				world.skybox = skybox;
			}
			else
			{
				world.skybox = std::nullopt;
			}
		}
	}

	void on_gui(klotter::Renderer* renderer) override
	{
		ImGui::Begin("Sample switcher");

		klotter::test_themes();

		ImGui::SeparatorText("Rendering");
		gui_msaa(renderer);

		gui_skybox();

		imgui_color("Clear color", &world.clear_color);
		simple_gamma_slider("Gamma/Brightness", &renderer->settings.gamma, -1.0f);
		ImGui::Checkbox("Use bloom", &renderer->settings.use_bloom);
		ImGui::DragFloat("Bloom cutoff", &renderer->settings.bloom_cutoff, FAC_SPEED);
		ImGui::SliderFloat("Softness", &renderer->settings.bloom_softness, 0.0f, 1.0f);
		ImGui::DragInt("Bloom blur steps", &renderer->settings.bloom_blur_steps);

		ImGui::SeparatorText("Effects");
		gui_effects_factors();
		effects.gui(&imgui_shader_cache);

		ImGui::SeparatorText("Stats");
		ImGui::DragFloat3("position", glm::value_ptr(camera.position));
		imgui_label("pitch", Str{} << camera.pitch);
		imgui_label("yaw", Str{} << camera.yaw);
		imgui_label("window size", Str{} << last_window_size.x << " | " << last_window_size.y);
		imgui_label("projected target", Str{} << projected_target.x << " | " << projected_target.y);

		ImGui::SeparatorText("Outline");
		gui_outline_toggle();

		ImGui::SeparatorText("Ambient lights");
		ImGui::PushID("ambient lights");
		gui_ambient_light();
		ImGui::PopID();

		ImGui::SeparatorText("Directional lights");
		ImGui::PushID("directional lights");
		gui_all_direction_lights();
		ImGui::PopID();

		ImGui::SeparatorText("Point lights");
		ImGui::PushID("point lights");
		gui_all_point_lights();
		ImGui::PopID();

		ImGui::SeparatorText("Frustum lights");
		ImGui::PushID("frustum lights");
		gui_all_frustum_lights();
		ImGui::PopID();


		ImGui::End();

		ImGui::ShowDemoWindow(nullptr);
	}
};

}  //  namespace examples

IMPLEMENT_MAIN(examples::LightsSample)
