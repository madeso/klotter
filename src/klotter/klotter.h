#pragma once

#include <memory>

#include "imgui.h"

#include "klotter/render/render.h"
#include "klotter/dependency_sdl.h"
#include "klotter/render/camera.h"

namespace klotter
{

struct App
{
	Camera camera;

	virtual void on_frame(Renderer*) = 0;
	virtual void on_gui(Renderer*) = 0;
	virtual void on_render(const glm::ivec2&, Renderer*, float) = 0;

	virtual ~App() = default;
};

using MakeAppFunction = std::unique_ptr<App> (*)(Renderer*);
int run_main(const RenderSettings& rs, MakeAppFunction make_app);

}  //  namespace klotter

#define IMPLEMENT_MAIN(APP) \
	int main(int, char**) \
	{ \
		return klotter::run_main( \
			APP::get_render_settings(), \
			[](Renderer* r) -> std::unique_ptr<App> { return std::make_unique<APP>(r); } \
		); \
	}
