#pragma once

#include "klotter/render/postproc.h"

#include "klotter/feature_flags.h"
#include "klotter/render/renderer.h"
#include "klotter/render/shader.h"
#include "klotter/render/shader_resource.h"

namespace klotter
{

/** \addtogroup postproc Post Processing
 *  @{
*/


/// @brief A source that "just" renders the world.
struct RenderWorld : RenderSource
{
	glm::ivec2 window_size;

	std::shared_ptr<FrameBuffer> msaa_buffer;
	std::shared_ptr<FrameBuffer> realized_buffer;
	std::shared_ptr<LoadedPostProcShader> shader;

	RenderWorld(const glm::ivec2 size, std::shared_ptr<LoadedPostProcShader> sh, int msaa_samples);

	void update(const PostProcArg& arg);

	void render(const PostProcArg& arg) override;
};


/// A tweakable shader property.
struct ShaderProp
{
	ShaderProp() = default;
	ShaderProp(const ShaderProp&) = delete;
	ShaderProp(ShaderProp&&) = delete;
	void operator=(const ShaderProp&) = delete;
	void operator=(ShaderProp&&) = delete;

	virtual ~ShaderProp() = default;

	virtual void use(const PostProcArg& a, ShaderProgram& shader) = 0;
	virtual void gui() = 0;
};

/// A float shader property that is dragged with the mouse.
struct FloatDragShaderProp : ShaderProp
{
	Uniform uniform;
	std::string name;
	float value;
	float speed;

	FloatDragShaderProp(const LoadedPostProcShader& shader, const std::string& n, float v, float s);

	void use(const PostProcArg&, ShaderProgram& shader) override;

	void gui() override;
};

/// A float shader property that is a slider.
struct FloatSliderShaderProp : ShaderProp
{
	Uniform uniform;
	std::string name;
	float value;
	float min;
	float max;

	FloatSliderShaderProp(
		const LoadedPostProcShader& shader, const std::string& n, float v, float mi, float ma
	);

	void use(const PostProcArg&, ShaderProgram& shader) override;

	void gui() override;
};

/// A effect that only performs a single step.
/// Most effects are simple, like invert, grayscale, etc. but some are more complex like blur.
struct SimpleEffect
	: FactorEffect
	, ShaderPropertyProvider
{
	std::string name;
	std::shared_ptr<LoadedPostProcShader> shader;
	std::vector<std::shared_ptr<ShaderProp>> properties;
	float time = 0.0f;

	SimpleEffect(std::string n, std::shared_ptr<LoadedPostProcShader> s);

	void add_float_drag_prop(const std::string& prop_name, float value, float speed);

	void add_float_slider_prop(const std::string& prop_name, float value, float min, float max);

	void gui() override;

	void update(float dt) override;

	void use_shader(const PostProcArg& a, const FrameBuffer& t) override;

	void build(const BuildArg& arg) override;
};

struct BlurEffect;

/// A shader property for the vertical blur step.
struct BlurVerticalProvider : ShaderPropertyProvider
{
	explicit BlurVerticalProvider(BlurEffect* b);

	BlurEffect* blur;
	void use_shader(const PostProcArg& a, const FrameBuffer& t) override;
};

/// A shader property for the horizontal blur step.
struct BurHorizontalProvider : ShaderPropertyProvider
{
	explicit BurHorizontalProvider(BlurEffect* b);

	BlurEffect* blur;
	void use_shader(const PostProcArg& a, const FrameBuffer& t) override;
};

/// A blur effect split into  a vertical and horizontal blur step.
struct BlurEffect : FactorEffect
{
	std::string name;
	BlurVerticalProvider vert_p;
	BurHorizontalProvider hori_p;
	std::shared_ptr<LoadedPostProcShader> vert;
	std::shared_ptr<LoadedPostProcShader> hori;

	Uniform blur_size_v;
	Uniform blur_size_h;

#if FF_HAS(BLUR_USE_GAUSS)
	Uniform std_dev_v;
	Uniform std_dev_h;
#endif

	float blur_size = 0.02f;
#if FF_HAS(BLUR_USE_GAUSS)
	float std_dev = 0.02f;
#endif

	BlurEffect(std::string n, std::shared_ptr<LoadedPostProcShader> v, std::shared_ptr<LoadedPostProcShader> h);

	void gui() override;

	void update(float) override;

	void use_vert_shader(const PostProcArg& a, const FrameBuffer& t) const;

	void use_hori_shader(const PostProcArg& a, const FrameBuffer& t);

	void build(const BuildArg& arg) override;
};

/**
 * @}
*/

}
