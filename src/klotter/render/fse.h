#pragma once

#include "klotter/render/texture.h"

namespace klotter
{
struct Texture2d;
struct Renderer;
struct Camera;
struct World;

/** \addtogroup render Renderer
 *  @{
*/

struct PostProcArg
{
	const World* world;
	glm::ivec2 window_size;
	const Camera* camera;
	Renderer* renderer;
};

struct RenderSource
{
	RenderSource() = default;

	RenderSource(const RenderSource&) = delete;
	RenderSource(RenderSource&&) = delete;
	void operator=(const RenderSource&) = delete;
	void operator=(RenderSource&&) = delete;

	virtual ~RenderSource() = default;
	virtual void render(const PostProcArg& arg) = 0;
};

struct ShaderPropertyProvider
{
	ShaderPropertyProvider() = default;

	ShaderPropertyProvider(const ShaderPropertyProvider&) = delete;
	ShaderPropertyProvider(ShaderPropertyProvider&&) = delete;
	void operator=(const ShaderPropertyProvider&) = delete;
	void operator=(ShaderPropertyProvider&&) = delete;

	virtual ~ShaderPropertyProvider() = default;
	virtual void use_shader(const PostProcArg& a, const FrameBuffer& t) = 0;
};

struct RenderTask : RenderSource
{
	std::shared_ptr<RenderSource> source;
	std::shared_ptr<FrameBuffer> fbo;
	ShaderPropertyProvider* effect;

	RenderTask(std::shared_ptr<RenderSource> s, std::shared_ptr<FrameBuffer> f, ShaderPropertyProvider* e);

	/// render internal fbo to a quad with a shader
	void render(const PostProcArg& arg) override;

	/// call render on linked source and render to fbo
	void update(const PostProcArg& arg);
};

/// A compiled full-screen-effect.
/// when compiled it could be:
/// * [render world to screen]
/// * [render world to fbo], [render fbo to screen with shader]
/// * [render world to fbo] [[render fbo to intermediate with shader] [render int to screen with shader]]
/// * [render world to fbo] [[render fbo to intermediate with shader] [render int to fbo with shader]] [render fbo to screen with shader]
struct CompiledStack
{
	/// start with a simple world, but depending on the current effect list, could be more...
	std::shared_ptr<RenderSource> last_source;

	std::vector<std::shared_ptr<RenderTask>> targets;
};

struct FrameBufferCache
{
	[[nodiscard]] std::shared_ptr<FrameBuffer> get(
		glm::ivec2 size, TextureEdge edge, TextureRenderStyle render_style, Transparency transperency
	) const;
};

struct BuildArg
{
	CompiledStack* builder;
	FrameBufferCache* fbo;
	glm::ivec2 window_size;
};


struct EffectStack;

struct Effect
{
	Effect() = default;

	Effect(const Effect&) = delete;
	Effect(Effect&&) = delete;
	void operator=(const Effect&) = delete;
	void operator=(Effect&&) = delete;

	virtual ~Effect() = default;

	virtual void build(const BuildArg& arg) = 0;
	virtual void update(float dt) = 0;
	virtual void gui() = 0;

	bool enabled() const;

   protected:

	void set_enabled(bool n);

   private:

	friend struct EffectStack;

	bool is_enabled = false;
	EffectStack* owner = nullptr;
};

struct FactorEffect : Effect
{
	FactorEffect();
	[[nodiscard]] float get_factor() const;
	void set_factor(float f);

   private:

	float factor = 0.0f;
};

/// The facade of the post-proc framework.
struct EffectStack
{
	bool dirty = true;
	std::optional<glm::ivec2> window_size;
	std::vector<std::shared_ptr<Effect>> effects;
	CompiledStack compiled;
	FrameBufferCache fbos;

	/// rebuilds stack if dirty, update all targets, then render the last_source
	void render(const PostProcArg& arg);
	void update(float dt) const;
	void gui() const;
};

/**
 * @}
*/

}  //  namespace klotter
