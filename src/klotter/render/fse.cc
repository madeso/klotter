#include "klotter/render/fse.h"

#include "imgui.h"

#include "klotter/assert.h"
#include "klotter/log.h"

#include "klotter/render/constants.h"
#include "klotter/render/opengl_utils.h"
#include "klotter/render/render.h"
#include "klotter/render/shader.h"
#include "klotter/render/shader_resource.h"
#include "klotter/render/statechanger.h"
#include "klotter/render/render.pimpl.h"

namespace klotter
{
struct ShaderProgram;

bool Effect::enabled() const
{
	return is_enabled;
}

void Effect::set_enabled(bool n)
{
	if (is_enabled == n) return;
	is_enabled = n;
	if (owner)
	{
		owner->dirty = true;
	}
}

struct RenderWorld : RenderSource
{
	glm::ivec2 window_size;

	std::shared_ptr<FrameBuffer> msaa_buffer;
	std::shared_ptr<FrameBuffer> realized_buffer;
	std::shared_ptr<LoadedPostProcShader> shader;

	RenderWorld(const glm::ivec2 size, std::shared_ptr<LoadedPostProcShader> sh, int msaa_samples)
		: window_size(size)
	{
		msaa_buffer = FrameBufferBuilder{size}
			.with_msaa(msaa_samples)
			.with_depth()
			.build();
		realized_buffer = FrameBufferBuilder{size}
			.build();
		shader = std::move(sh);
	}

	void update(const PostProcArg& arg)
	{
		// render into msaa buffer
		{
			auto bound = BoundFbo{msaa_buffer};
			arg.renderer->render_world(window_size, *arg.world, *arg.camera);
		}

		// copy msaa buffer to realized
		resolve_multisampled_buffer(*msaa_buffer, realized_buffer.get());
	}

	void render(const PostProcArg& arg) override
	{
		// render realized (with shader)
		StateChanger{&arg.renderer->pimpl->states}
			.cull_face(false)
			.stencil_test(false)
			.depth_test(false)
			.depth_mask(false)
			.blending(false);

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->program->use();
		bind_texture_2d(&arg.renderer->pimpl->states, shader->texture, *realized_buffer);

		render_geom(*arg.renderer->pimpl->full_screen_geom);
	}
};

RenderTask::RenderTask(std::shared_ptr<RenderSource> s, std::shared_ptr<FrameBuffer> f, ShaderPropertyProvider* e)
	: source(std::move(s))
	, fbo(std::move(f))
	, effect(e)
{
	ASSERT(effect);
}

void RenderTask::render(const PostProcArg& arg)
{
	ASSERT(effect);

	StateChanger{&arg.renderer->pimpl->states}
		.cull_face(false)
		.stencil_test(false)
		.depth_test(false)
		.depth_mask(false)
		.blending(false);

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	effect->use_shader(arg, *fbo);
	render_geom(*arg.renderer->pimpl->full_screen_geom);
}

void RenderTask::update(const PostProcArg& arg)
{
	auto bound = BoundFbo{fbo};
	set_gl_viewport({fbo->width, fbo->height});
	source->render(arg);
}

void EffectStack::update(float dt) const
{
	for (auto e: effects)
	{
		if (e->is_enabled == false)
		{
			continue;
		}

		e->update(dt);
	}
}

void EffectStack::render(const PostProcArg& arg)
{
	// set the owner, if any new then we are dirty
	for (const auto& e: effects)
	{
		if (e->owner != this)
		{
			e->owner = this;
			dirty = true;
		}
	}

	auto render_world = render_world_ref.lock();
	if (render_world == nullptr)
	{
		dirty = true;
	}

	if (window_size.has_value() == false || *window_size != arg.window_size)
	{
		window_size = arg.window_size;
		dirty = true;
	}

	// if dirty, update the compiled state
	if (dirty)
	{
		dirty = false;
		LOG_INFO("Building effects stack");

		compiled.targets.clear();

		auto created_world
			= std::make_shared<RenderWorld>(arg.window_size, arg.renderer->pimpl->shaders_resources.pp_always, arg.renderer->settings.msaa);
		compiled.last_source = created_world;
		render_world_ref = created_world;
		render_world = created_world;

		for (auto& e: effects)
		{
			if (e->is_enabled)
			{
				e->build({&compiled, arg.window_size});
			}
		}
	}

	// the stack is now compiled
	// before rendering, update all targets/fbos and present
	render_world->update(arg);
	for (const auto& action: compiled.targets)
	{
		action->update(arg);
	}

	// render the final image to the screen
	set_gl_viewport(arg.window_size);
	compiled.last_source->render(arg);
}

void EffectStack::gui() const
{
	int index = 0;
	for (const auto& e: effects)
	{
		ImGui::PushID(index);
		e->gui();
		ImGui::PopID();
		index += 1;
	}
}

FactorEffect::FactorEffect()
{
	set_enabled(false);
}

float FactorEffect::get_factor() const
{
	return factor;
}

void FactorEffect::set_factor(float f)
{
	factor = f;
	set_enabled(factor > ALMOST_ZERO);
}

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

struct FloatDragShaderProp : ShaderProp
{
	Uniform uniform;
	std::string name;
	float value;
	float speed;

	FloatDragShaderProp(const LoadedPostProcShader& shader, const std::string& n, float v, float s)
		: uniform(shader.program->get_uniform(n))
		, name(n)
		, value(v)
		, speed(s)
	{
	}

	void use(const PostProcArg&, ShaderProgram& shader) override
	{
		shader.set_float(uniform, value);
	}

	void gui() override
	{
		ImGui::DragFloat(name.c_str(), &value, speed);
	}
};

struct FloatSliderShaderProp : ShaderProp
{
	Uniform uniform;
	std::string name;
	float value;
	float min;
	float max;

	FloatSliderShaderProp(
		const LoadedPostProcShader& shader, const std::string& n, float v, float mi, float ma
	)
		: uniform(shader.program->get_uniform(n))
		, name(n)
		, value(v)
		, min(mi)
		, max(ma)
	{
	}

	void use(const PostProcArg&, ShaderProgram& shader) override
	{
		shader.set_float(uniform, value);
	}

	void gui() override
	{
		ImGui::SliderFloat(name.c_str(), &value, min, max);
	}
};

struct SimpleEffect
	: FactorEffect
	, ShaderPropertyProvider
{
	std::string name;
	std::shared_ptr<LoadedPostProcShader> shader;
	std::vector<std::shared_ptr<ShaderProp>> properties;
	float time = 0.0f;

	SimpleEffect(std::string n, std::shared_ptr<LoadedPostProcShader> s)
		: name(std::move(n))
		, shader(std::move(s))
	{
		ASSERT(shader->factor.has_value());
	}

	void add_float_drag_prop(const std::string& prop_name, float value, float speed)
	{
		properties.emplace_back(std::make_shared<FloatDragShaderProp>(*shader, prop_name, value, speed));
	}

	void add_float_slider_prop(const std::string& prop_name, float value, float min, float max)
	{
		properties.emplace_back(std::make_shared<FloatSliderShaderProp>(*shader, prop_name, value, min, max));
	}

	void gui() override
	{
		if (properties.empty())
		{
			return;
		}

		int index = 0;

		if (ImGui::CollapsingHeader(name.c_str()) == false)
		{
			return;
		}

		for (auto& p: properties)
		{
			ImGui::PushID(index);
			p->gui();
			ImGui::PopID();
			index += 1;
		}
	}

	void update(float dt) override
	{
		time += dt;
	}

	void use_shader(const PostProcArg& a, const FrameBuffer& t) override
	{
		shader->program->use();
		if (shader->factor)
		{
			shader->program->set_float(*shader->factor, get_factor());
		}
		if (shader->resolution)
		{
			shader->program->set_vec2(*shader->resolution, a.window_size);
		}
		if (shader->time)
		{
			shader->program->set_float(*shader->time, time);
		}
		for (auto& p: properties)
		{
			p->use(a, *shader->program);
		}
		bind_texture_2d(&a.renderer->pimpl->states, shader->texture, t);
	}

	void build(const BuildArg& arg) override
	{
		time = 0.0f;

		auto fbo = FrameBufferBuilder{arg.window_size}.build();

		auto src = arg.builder->last_source;
		auto target = std::make_shared<RenderTask>(src, fbo, this);

		arg.builder->targets.emplace_back(target);
		arg.builder->last_source = target;
	}
};

struct BlurEffect;

struct VertProvider : ShaderPropertyProvider
{
	explicit VertProvider(BlurEffect* b)
		: blur(b)
	{
	}

	BlurEffect* blur;
	void use_shader(const PostProcArg& a, const FrameBuffer& t) override;
};

struct HoriProvider : ShaderPropertyProvider
{
	explicit HoriProvider(BlurEffect* b)
		: blur(b)
	{
	}

	BlurEffect* blur;
	void use_shader(const PostProcArg& a, const FrameBuffer& t) override;
};

struct BlurEffect : FactorEffect
{
	std::string name;
	VertProvider vert_p;
	HoriProvider hori_p;
	std::shared_ptr<LoadedPostProcShader> vert;
	std::shared_ptr<LoadedPostProcShader> hori;

	Uniform blur_size_v;
	Uniform blur_size_h;

#if HAS(BLUR_USE_GAUSS)
	Uniform std_dev_v;
	Uniform std_dev_h;
#endif

	float blur_size = 0.02f;
#if HAS(BLUR_USE_GAUSS)
	float std_dev = 0.02f;
#endif

	BlurEffect(std::string n, std::shared_ptr<LoadedPostProcShader> v, std::shared_ptr<LoadedPostProcShader> h)
		: name(std::move(n))
		, vert_p(this)
		, hori_p(this)
		, vert(std::move(v))
		, hori(std::move(h))
		, blur_size_v(vert->program->get_uniform("u_blur_size"))
		, blur_size_h(hori->program->get_uniform("u_blur_size"))
#if HAS(BLUR_USE_GAUSS)
		, std_dev_v(vert->program->get_uniform("u_std_dev"))
		, std_dev_h(hori->program->get_uniform("u_std_dev"))
#endif
	{
		ASSERT(vert->factor.has_value());
		ASSERT(hori->factor.has_value());
	}

	void gui() override
	{
		if (ImGui::CollapsingHeader(name.c_str()) == false)
		{
			return;
		}

		ImGui::SliderFloat("Blur size", &blur_size, 0.0f, 0.2f);
#if HAS(BLUR_USE_GAUSS)
		ImGui::SliderFloat("Standard deviation", &std_dev, 0.0f, 0.1f);
#endif
	}

	void update(float) override
	{
		// no update needed
	}

	void use_vert_shader(const PostProcArg& a, const FrameBuffer& t) const
	{
		vert->program->use();
		ASSERT(vert->factor);
		vert->program->set_float(*vert->factor, get_factor());
		vert->program->set_float(blur_size_v, blur_size);
#if HAS(BLUR_USE_GAUSS)
		vert->program->set_float(std_dev_v, std_dev);
#endif
		bind_texture_2d(&a.renderer->pimpl->states, vert->texture, t);
	}

	void use_hori_shader(const PostProcArg& a, const FrameBuffer& t)
	{
		hori->program->use();
		ASSERT(hori->factor);
		hori->program->set_float(*hori->factor, get_factor());
		ASSERT(hori->resolution);
		hori->program->set_vec2(*hori->resolution, a.window_size);
		hori->program->set_float(blur_size_h, blur_size);
#if HAS(BLUR_USE_GAUSS)
		hori->program->set_float(std_dev_h, std_dev);
#endif
		bind_texture_2d(&a.renderer->pimpl->states, hori->texture, t);
	}

	void build(const BuildArg& arg) override
	{
		auto src = arg.builder->last_source;

		// todo(Gustav): modify resolution to get better blur and at a lower cost!

		// step 1: vertical
		auto fbo_v = FrameBufferBuilder{arg.window_size}.build();
		auto target_v = std::make_shared<RenderTask>(src, fbo_v, &vert_p);
		arg.builder->targets.emplace_back(target_v);

		// step 2: horizontal
		auto fbo_h = FrameBufferBuilder{arg.window_size}.build();
		auto target_h = std::make_shared<RenderTask>(target_v, fbo_h, &hori_p);
		arg.builder->targets.emplace_back(target_h);

		// done
		arg.builder->last_source = target_h;
	}
};

void VertProvider::use_shader(const PostProcArg& a, const FrameBuffer& t)
{
	blur->use_vert_shader(a, t);
}

void HoriProvider::use_shader(const PostProcArg& a, const FrameBuffer& t)
{
	blur->use_hori_shader(a, t);
}

std::shared_ptr<FactorEffect> Renderer::make_invert_effect()
{
	return std::make_shared<SimpleEffect>("Invert", pimpl->shaders_resources.pp_invert);
}

std::shared_ptr<FactorEffect> Renderer::make_grayscale_effect()
{
	return std::make_shared<SimpleEffect>("Grayscale", pimpl->shaders_resources.pp_grayscale);
}

std::shared_ptr<FactorEffect> Renderer::make_damage_effect()
{
	auto r = std::make_shared<SimpleEffect>("Damage", pimpl->shaders_resources.pp_damage);
	r->add_float_drag_prop("u_vignette_radius", 0.13f, 0.01f);
	r->add_float_slider_prop("u_vignette_smoothness", 1.0f, 0.001f, 1.0f);
	r->add_float_slider_prop("u_vignette_darkening", 1.0f, 0.0f, 1.0f);
	r->add_float_drag_prop("u_noise_scale", 25.0f, 1.0f);
	return r;
}

std::shared_ptr<FactorEffect> Renderer::make_blur_effect()
{
	return std::make_shared<BlurEffect>("Blur", pimpl->shaders_resources.pp_blurv, pimpl->shaders_resources.pp_blurh);
}

}  //  namespace klotter
