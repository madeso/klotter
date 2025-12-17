#pragma once

#include "klotter/render/camera.h"

namespace klotter
{

struct World;
struct DirectionalLight;

OrthoCamera shadow_cam_from_light(const DirectionalLight& light, const World& world, const Camera& camera);

CompiledCamera calculate_tight_fitting_camera_around_perspective(
	const CompiledCamera& perspective, const glm::vec3& light_direction
);
    
}
