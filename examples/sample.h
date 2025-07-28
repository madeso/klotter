#pragma once

#include "klotter/klotter.h"

namespace examples
{


struct Sample
{
	virtual ~Sample() = default;

	virtual void on_render(
		const glm::ivec2& window_size, klotter::Renderer* ren, klotter::Camera* cam, float dt
	) = 0;
	virtual void on_gui(klotter::Renderer*, klotter::Camera* cam) = 0;
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

	void set_selected_sample(klotter::Renderer* renderer, std::size_t new_selected_sample);

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
			}
		});
	}

	void on_frame(klotter::Renderer*) override;
	void on_gui(klotter::Renderer*) override;
	void on_render(const glm::ivec2&, klotter::Renderer*, float) override;
};


}  //  namespace examples
