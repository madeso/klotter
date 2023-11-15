#pragma once

#include "klotter/klotter.h"

namespace examples
{


struct Sample
{
	virtual ~Sample() = default;

	virtual void on_render(float dt) = 0;
	virtual void on_gui() = 0;
};

struct DefinedSample
{
	std::string name;
	std::unique_ptr<Sample> created_sample;
	klotter::Camera stored_camera;

	// delayed sample creation
	std::unique_ptr<Sample> (*create)(klotter::Renderer*, klotter::Camera*);
};

struct SampleApp : klotter::App
{
	std::vector<DefinedSample> samples;
	std::optional<std::size_t> selected_sample;
	std::optional<std::size_t> active_sample;

	void set_selected_sample(std::size_t new_selected_sample);

	template<typename T>
	void add_sample(const std::string& name)
	{
		samples.emplace_back(DefinedSample{
			name,
			nullptr,
			{},
			[](klotter::Renderer* r, klotter::Camera* c) -> std::unique_ptr<Sample>
			{
				return std::make_unique<T>(r, c);
			}});
	}

	void on_frame() override;
	void on_gui() override;
	void on_render(float) override;
};


}  //  namespace examples
