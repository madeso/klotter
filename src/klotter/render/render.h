#pragma once

#include "klotter/render/camera.h"


namespace klotter
{



struct CompiledGeom
{
};

using GeomPtr = std::shared_ptr<CompiledGeom>;
GeomPtr make_BoxGeometry(float boxWidth, float boxHeight, float boxDepth);



struct Material
{
};
using MaterialPtr = std::shared_ptr<Material>;
MaterialPtr make_BasicMaterial();



struct MeshInstance
{
    GeomPtr geom;
    MaterialPtr material;

    glm::vec3 position;
    glm::quat rotation;
};
using MeshInstancePtr = std::shared_ptr<MeshInstance>;
MeshInstancePtr make_MeshInstance(GeomPtr geom, MaterialPtr material);


glm::vec3 color_from_hex(int hex);
        

struct Scene
{
    std::vector<MeshInstancePtr> meshes;
};


void render(const Scene& scene, const Camera& camera);



}