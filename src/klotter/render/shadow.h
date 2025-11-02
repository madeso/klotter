#pragma once

#include "klotter/render/camera.h"

namespace klotter
{

struct World;
struct DirectionalLight;

OrthoCamera shadow_cam_from_light(const DirectionalLight& light, const World& world, const Camera& camera);
    
}
