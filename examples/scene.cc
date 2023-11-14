#include "scene.h"

namespace examples
{


void SceneApp::select_scene(std::size_t new_scene)
{
	auto last_selected = selected_scene;
	scene_index = new_scene;

	if (types[scene_index].created_scene == nullptr)
	{
		camera = {};
		types[scene_index].created_scene = types[scene_index].create(&renderer, &camera);
		types[scene_index].created_scene->stored_camera = camera;
	}
	selected_scene = types[scene_index].created_scene;

	if (last_selected != selected_scene)
	{
		if (last_selected != nullptr)
		{
			last_selected->stored_camera = camera;
		}

		if (selected_scene)
		{
			camera = selected_scene->stored_camera;
		}
	}
}

void SceneApp::on_frame()
{
	if (selected_scene == nullptr)
	{
		select_scene(types.size() - 1);
	}
	active_scene = selected_scene;
}

void SceneApp::on_render(float dt)
{
	active_scene->on_render(dt);
}

void SceneApp::on_gui()
{
	ImGui::Begin("Scene switcher");

	if (ImGui::BeginCombo("Scene", types[scene_index].name.c_str(), ImGuiComboFlags_HeightRegular))
	{
		for (std::size_t si = 0; si < types.size(); si += 1)
		{
			if (ImGui::Selectable(types[si].name.c_str(), si == scene_index))
			{
				select_scene(si);
			}
		}
		ImGui::EndCombo();
	}

	active_scene->on_gui();

	ImGui::End();

	ImGui::ShowDemoWindow(nullptr);
}

}  //  namespace examples
