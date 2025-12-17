#pragma once

#include "klotter/render/camera.h"

namespace klotter
{

struct World;
struct DirectionalLight;
struct RenderSettings;

CompiledCamera compile_the_shadow_camera(
	const Camera& camera,
	const glm::ivec2& window_size,
	const DirectionalLight& light,
	const RenderSettings& settings,
	const World& world
);
    
}
