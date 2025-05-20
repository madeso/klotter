#pragma once

#include "klotter/render/assets.h"
#include "klotter/render/debug.h"
#include "klotter/render/fse.h"
#include "klotter/render/material.h"
#include "klotter/render/render_settings.h"
#include "klotter/render/vertex_layout.h"
#include "klotter/render/world.h"

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/

struct RendererPimpl;

/// The renderering engine.
struct Renderer
{
	RenderSettings settings;
	Assets assets;
	DebugRender debug;

	std::unique_ptr<RendererPimpl> pimpl;

	explicit Renderer(const RenderSettings& settings);
	~Renderer();

	std::shared_ptr<UnlitMaterial> make_unlit_material();
	std::shared_ptr<DefaultMaterial> make_default_material();

	CompiledGeomVertexAttributes unlit_geom_layout() const;
	CompiledGeomVertexAttributes default_geom_layout() const;

	std::shared_ptr<FactorEffect> make_invert_effect();
	std::shared_ptr<FactorEffect> make_grayscale_effect();
	std::shared_ptr<FactorEffect> make_damage_effect();
	std::shared_ptr<FactorEffect> make_blur_effect();

	Skybox make_skybox(std::shared_ptr<TextureCubemap> texture) const;

	/// verify that the renderer was fully loaded
	bool is_loaded() const;

	/// doesn't set the size, prefer EffectStack::render
	void render_world(const glm::ivec2& window_size, const World&, const Camera&);
};

/**
 * @}
*/

}  //  namespace klotter
