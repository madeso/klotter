#pragma once
#include <unordered_set>

#include "klotter/scurve.h"

#include "klotter/render/material.h"
#include "klotter/render/vertex_layout.h"

namespace klotter
{
struct Geom;

/** \addtogroup render Renderer
 *  @{
*/


/// Represents a Geom on the GPU.
struct CompiledGeom
{
	u32 vbo;
	u32 vao;
	u32 ebo;
	i32 number_of_triangles;
	std::unordered_set<VertexType> debug_types;

	explicit CompiledGeom(u32, u32, u32, const CompiledGeomVertexAttributes&, i32);
	~CompiledGeom();

	CompiledGeom(const CompiledGeom&) = delete;
	CompiledGeom(CompiledGeom&&) = delete;
	void operator=(const CompiledGeom&) = delete;
	void operator=(CompiledGeom&&) = delete;
};

/// Represents a Geom on the GPU, instanced on a transform.
struct CompiledGeom_TransformInstance
{
	u32 instance_vbo;
	std::size_t max_instances;

	u32 vbo;
	u32 vao;
	u32 ebo;
	i32 number_of_triangles;
	std::unordered_set<VertexType> debug_types;

	explicit CompiledGeom_TransformInstance(u32, std::size_t, u32, u32, u32, const CompiledGeomVertexAttributes&, i32);
	~CompiledGeom_TransformInstance();

	CompiledGeom_TransformInstance(const CompiledGeom_TransformInstance&) = delete;
	CompiledGeom_TransformInstance(CompiledGeom_TransformInstance&&) = delete;
	void operator=(const CompiledGeom_TransformInstance&) = delete;
	void operator=(CompiledGeom_TransformInstance&&) = delete;
};

std::shared_ptr<CompiledGeom> compile_geom(DEBUG_LABEL_ARG_MANY const Geom&, const CompiledGeomVertexAttributes& layout);
std::shared_ptr<CompiledGeom_TransformInstance> compile_geom_with_transform_instance(
	DEBUG_LABEL_ARG_MANY
	const Geom&, const CompiledGeomVertexAttributes& layout, std::size_t max_instances
);

// todo(Gustav): merge with CameraVectors... this has a better name
struct LocalAxis
{
	glm::vec3 x;
	glm::vec3 y;
	glm::vec3 z;
};

/// Defines the billboarding algorithm to use
enum class Billboarding
{
	none,
	screen,
	screen_fast,
	axial_y,
	axial_y_fast
};

/// Stores Geom + Material (aka a mesh) and its current transform.
struct MeshInstance
{
	std::shared_ptr<CompiledGeom> geom;
	std::shared_ptr<Material> material;

	std::optional<Color> outline;

	glm::vec3 world_position = glm::vec3{0.0f};
	glm::vec3 rotation = glm::vec3{0.0f};  ///< yaw pitch roll
	Billboarding billboarding = Billboarding::none;	 ///< if not none, rotation is ignored

	LocalAxis get_local_axis() const;
};

std::shared_ptr<MeshInstance> make_mesh_instance(std::shared_ptr<CompiledGeom> geom, std::shared_ptr<Material> mat);

/// Stores Geom + Material (aka a mesh) and its current transform but instanced for faster rendering.
struct MeshInstance_TransformInstanced
{
	std::shared_ptr<CompiledGeom_TransformInstance> geom;
	std::shared_ptr<Material> material;

	//todo(Gustav): add outline

	std::vector<glm::mat4> world_from_locals;
};

std::shared_ptr<MeshInstance_TransformInstanced> make_mesh_instance(
	std::shared_ptr<CompiledGeom_TransformInstance> geom, std::shared_ptr<Material> mat
);

void render_geom(const CompiledGeom& geom);
void render_geom_instanced(const MeshInstance_TransformInstanced& instanced);


/// A directional light,
struct DirectionalLight
{
	glm::vec3 direction = {1.0f, 0.0f, 0.0f};

	Color color = colors::white;
	float specular_strength = 1.0f;
	float diffuse_strength = 1.0f;
};

/// A point light.
struct PointLight
{
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	float min_range = 10.0f;
	float max_range = 20.0f;
	SCurve curve = SCurve::light_curve();	///< curve and ui, needs to be trimmed down for actual renderer

	Color color = colors::white;
	float specular_strength = 1.0f;
	float diffuse_strength = 1.0f;
};

/// Like a spotlight but can be positioned with a transform, supports light cookies and can use a "viewfrustom" when culling.
struct FrustumLight
{
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	float yaw = 0.0f;
	float pitch = 0.0f;
	float fov = 10.0f;	// in angles
	float aspect = 1.0;

	float min_range = 10.0f;
	float max_range = 20.0f;
	SCurve curve = SCurve::light_curve();  ///< curve and ui, needs to be trimmed down for actual renderer

	Color color = colors::white;
	float specular_strength = 1.0f;
	float diffuse_strength = 1.0f;

	std::shared_ptr<Texture2d> cookie;	// if null, pure white is used
};

/// All lights in a world.
struct Lights
{
	Color color = colors::white;
	float ambient_strength = 0.029f;

	std::vector<DirectionalLight> directional_lights;
	std::vector<PointLight> point_lights;
	std::vector<FrustumLight> frustum_lights;
};

/// A skybox model with a cubemap texture.
struct Skybox
{
	std::shared_ptr<CompiledGeom> geom = nullptr;
	std::shared_ptr<TextureCubemap> cubemap = nullptr;
};

/// A list of objects to render.
/// This is also sometimes known as a scene.
struct World
{
	std::vector<std::shared_ptr<MeshInstance>> meshes;
	std::vector<std::shared_ptr<MeshInstance_TransformInstanced>> instances;
	Lights lights;

	std::optional<Skybox> skybox;
};

/**
 * @}
*/

}  //  namespace klotter
