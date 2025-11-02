#include "klotter/render/shadow.h"

#include "klotter/render/world.h"

namespace klotter
{

 
OrthoCamera shadow_cam_from_light(const DirectionalLight& light, const World& world, const Camera& camera)
{
	auto cam = OrthoCamera{};
	const auto light_local_space = create_vectors(light);
	const auto shadow_offset = light_local_space.front * world.lights.shadow_offset;
	cam.position = camera.position - shadow_offset;
	cam.pitch = light.pitch;
	cam.yaw = light.yaw;
	cam.near = world.lights.shadow_near;
	cam.far = world.lights.shadow_far;
	cam.size = world.lights.shadow_size;
	return cam;
}



}