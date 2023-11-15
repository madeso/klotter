#include "sample.h"

namespace examples
{


void SampleApp::set_selected_sample(std::size_t new_selected_sample)
{
	auto last_selected = selected_sample;
	sample_index = new_selected_sample;

	if (samples[sample_index].created_sample == nullptr)
	{
		camera = {};
		samples[sample_index].created_sample = samples[sample_index].create(&renderer, &camera);
		samples[sample_index].created_sample->stored_camera = camera;
	}
	selected_sample = samples[sample_index].created_sample;

	if (last_selected != selected_sample)
	{
		if (last_selected != nullptr)
		{
			last_selected->stored_camera = camera;
		}

		if (selected_sample)
		{
			camera = selected_sample->stored_camera;
		}
	}
}

void SampleApp::on_frame()
{
	if (selected_sample == nullptr)
	{
		set_selected_sample(samples.size() - 1);
	}
	active_sample = selected_sample;
}

void SampleApp::on_render(float dt)
{
	active_sample->on_render(dt);
}

void SampleApp::on_gui()
{
	ImGui::Begin("Sample switcher");

	if (ImGui::BeginCombo(
			"Sample", samples[sample_index].name.c_str(), ImGuiComboFlags_HeightRegular
		))
	{
		for (std::size_t si = 0; si < samples.size(); si += 1)
		{
			if (ImGui::Selectable(samples[si].name.c_str(), si == sample_index))
			{
				set_selected_sample(si);
			}
		}
		ImGui::EndCombo();
	}

	active_sample->on_gui();

	ImGui::End();

	ImGui::ShowDemoWindow(nullptr);
}

}  //  namespace examples
