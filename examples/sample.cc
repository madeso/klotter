#include "sample.h"

#include "klotter/im_colors.h"

namespace examples
{


void SampleApp::set_selected_sample(std::size_t new_selected_sample)
{
	auto last_selected = selected_sample;
	selected_sample = new_selected_sample;

	if (samples[*selected_sample].created_sample == nullptr)
	{
		auto& def = samples[*selected_sample];
		def.stored_camera = {};
		def.created_sample = def.create(&renderer, &def.stored_camera);
	}

	if (last_selected != selected_sample)
	{
		if (last_selected)
		{
			samples[*last_selected].stored_camera = camera;
		}

		if (selected_sample)
		{
			camera = samples[*selected_sample].stored_camera;
		}
	}
}

void SampleApp::on_frame()
{
	if (selected_sample.has_value() == false)
	{
		set_selected_sample(samples.size() - 1);
	}
	active_sample = selected_sample;
}

void SampleApp::on_render(float dt)
{
	if (active_sample)
	{
		samples[*active_sample].created_sample->on_render(&renderer, &camera, dt);
	}
}

void SampleApp::on_gui()
{
	ImGui::Begin("Sample switcher");

	for (std::size_t si = 0; si < samples.size(); si += 1)
	{
		if (si != 0)
		{
			ImGui::SameLine();
		}

		const auto is_selected = selected_sample && si == *selected_sample;
		const auto& color = is_selected ? klotter::imgui::red : klotter::imgui::gray;
		ImGui::PushStyleColor(ImGuiCol_Button, color[6]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color[7]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, color[5]);
		if (ImGui::Button(samples[si].name.c_str()))
		{
			set_selected_sample(si);
		}
		ImGui::PopStyleColor(3);
	}

	if (active_sample)
	{
		samples[*active_sample].created_sample->on_gui(&camera);
	}

	ImGui::End();

	ImGui::ShowDemoWindow(nullptr);
}

}  //  namespace examples
