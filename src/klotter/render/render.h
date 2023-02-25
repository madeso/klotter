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
MaterialPtr make_MeshBasicMaterial();



struct Mesh
{
    GeomPtr geom;
    MaterialPtr material;

    glm::vec3 position;
    glm::quat rotation;
};
using MeshPtr = std::shared_ptr<Mesh>;
MeshPtr make_Mesh(GeomPtr geom, MaterialPtr material);


glm::vec3 color_from_hex(int hex);
        

struct Scene
{
    std::vector<MeshPtr> meshes;
};


void render(const Scene& scene, const Camera& camera);



}