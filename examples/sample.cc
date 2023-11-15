#include "sample.h"

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
		samples[*active_sample].created_sample->on_render(dt);
	}
}

void SampleApp::on_gui()
{
	ImGui::Begin("Sample switcher");

	if (selected_sample)
	{
		if (ImGui::BeginCombo(
				"Sample", samples[*selected_sample].name.c_str(), ImGuiComboFlags_HeightRegular
			))
		{
			for (std::size_t si = 0; si < samples.size(); si += 1)
			{
				if (ImGui::Selectable(samples[si].name.c_str(), si == *selected_sample))
				{
					set_selected_sample(si);
				}
			}
			ImGui::EndCombo();
		}
	}

	if (active_sample)
	{
		samples[*active_sample].created_sample->on_gui();
	}

	ImGui::End();

	ImGui::ShowDemoWindow(nullptr);
}

}  //  namespace examples
