#include "klotter/render/render.h"

namespace klotter
{


// move to member function of app to access current window size
void render(const Scene& scene, const Camera& camera)
{
}

GeomPtr make_BoxGeometry(float boxWidth, float boxHeight, float boxDepth)
{
    return std::make_shared<CompiledGeom>();
}

MaterialPtr make_BasicMaterial()
{
    return std::make_shared<Material>();
}

MeshInstancePtr make_MeshInstance(GeomPtr geom, MaterialPtr material)
{
    auto mesh = std::make_shared<MeshInstance>();
    mesh->geom = geom;
    mesh->material = material;
    return mesh;
}


glm::vec3 color_from_hex(int hex)
{
    return {0, 0, 0};
}



}