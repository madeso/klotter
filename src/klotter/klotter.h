#pragma once


#include "klotter/render/camera.h"
#include "klotter/dependency_sdl.h"


namespace klotter
{

struct App
{
    virtual ~App() = default;

    virtual void on_render(float dt) = 0;
};

using MakeAppFunction = std::function<std::unique_ptr<App>()>;
int run_main(MakeAppFunction make_app);



struct CompiledGeom
{
};
using GeomPtr = std::shared_ptr<CompiledGeom>;
GeomPtr make_BoxGeometry(float boxWidth, float boxHeight, float boxDepth);



struct Material
{
    glm::vec3 color;
};
using MaterialPtr = std::shared_ptr<Material>;
MaterialPtr make_MeshBasicMaterial();



struct Mesh
{
    GeomPtr geom;
    MaterialPtr material;
    glm::vec3 rotation;
};
using MeshPtr = std::shared_ptr<Mesh>;
MeshPtr make_Mesh(GeomPtr geom, MaterialPtr material);


glm::vec3 color_from_hex(int hex);
        

struct Scene
{
    std::vector<MeshPtr> meshes;

    void add(MeshPtr mesh);
};


void render(const Scene& scene, const Camera& camera);

}


#define IMPLEMENT_MAIN(APP) int main(int, char**)\
{ return klotter::run_main([](){ return std::make_unique<APP>();}); }

