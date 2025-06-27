#pragma once

#include "klotter/render/assets.h"
#include "klotter/render/debug.h"
#include "klotter/render/postproc.h"
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

/// The rendering engine.
struct Renderer
{
	RenderSettings settings;
	Assets assets;
	DebugRender debug;

	std::unique_ptr<RendererPimpl> pimpl;

	explicit Renderer(const RenderSettings& settings);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	void operator=(const Renderer&) = delete;
	void operator=(Renderer&&) = delete;

	std::shared_ptr<UnlitMaterial> make_unlit_material();
	std::shared_ptr<DefaultMaterial> make_default_material();

	[[nodiscard]] CompiledGeomVertexAttributes unlit_geom_layout() const;
	[[nodiscard]] CompiledGeomVertexAttributes default_geom_layout() const;

	std::shared_ptr<FactorEffect> make_invert_effect() const;
	std::shared_ptr<FactorEffect> make_grayscale_effect() const;
	std::shared_ptr<FactorEffect> make_damage_effect() const;
	std::shared_ptr<FactorEffect> make_blur_effect() const;

	[[nodiscard]] Skybox make_skybox(std::shared_ptr<TextureCubemap> texture) const;

	/// verify that the renderer was fully loaded
	[[nodiscard]] bool is_loaded() const;

	/// doesn't set the size, prefer EffectStack::render
	void render_world(const glm::ivec2& window_size, const World&, const Camera&);
};

/**
 * @}
*/

}  //  namespace klotter
