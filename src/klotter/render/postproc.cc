#include "klotter/render/postproc.h"

#include "klotter/assert.h"
#include "klotter/feature_flags.h"
#include "klotter/log.h"
#include "klotter/str.h"

#include "klotter/render/opengl_utils.h"
#include "klotter/render/postproc.internal.h"
#include "klotter/render/renderer.h"
#include "klotter/render/renderer.pimpl.h"
#include "klotter/render/shader_resource.h"
#include "klotter/render/shader.h"
#include "klotter/render/statechanger.h"
#include "klotter/render/ui.h"

#include "imgui.h"


namespace klotter
{


///////////////////////////////////////////////////////////////////////////////////////////////////
// Effect

bool Effect::enabled() const
{
	return is_enabled;
}

void Effect::set_enabled(bool n)
{
	if (is_enabled == n)
	{
		return;
	}

	is_enabled = n;
	if (owner != nullptr)
	{
		owner->dirty = true;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// RenderWorld

// todo(Gustav): should this be a user config option? evaluate higher/lower bits, probably 16 or 32 since it needs to be floating point for hdr
constexpr ColorBitsPerPixel render_world_color_bits_per_pixel = ColorBitsPerPixel::use_16;

std::optional<BloomRender> build_bloom(const glm::ivec2& size, ExtractShader* sh, PingPongBlurShader* ping_sh)
{
	if (sh == nullptr) { return std::nullopt; }

	auto bloom_buffer = FrameBufferBuilder{size}
		 .build(USE_DEBUG_LABEL("bloom extraction buffer"));

	auto ping_one = FrameBufferBuilder{size}
		 .build(USE_DEBUG_LABEL("ping-pong bloom buffer one"));
	auto ping_two = FrameBufferBuilder{size}
		 .build(USE_DEBUG_LABEL("ping-pong bloom buffer two"));

	return BloomRender{sh, bloom_buffer, ping_sh, {ping_one, ping_two}};
}

RenderWorld::RenderWorld(const glm::ivec2 size, RealizeShader* re_sh, ExtractShader* ex_sh, PingPongBlurShader* ping_sh, int msaa_samples, bool* h, float* e)
	: window_size(size)
	, use_hdr(h)
	, exposure(e)
	, msaa_buffer(FrameBufferBuilder{size}
		.with_msaa(msaa_samples)
		.with_color_bits(render_world_color_bits_per_pixel)
		.with_depth()
		.with_stencil()
		.build(USE_DEBUG_LABEL("msaa buffer")))
	, realized_buffer(FrameBufferBuilder{size}
		.with_color_bits(render_world_color_bits_per_pixel)
		.build(USE_DEBUG_LABEL("realized msaa buffer")))
	, realize_shader(re_sh)
	, bloom_render(build_bloom(size, ex_sh, ping_sh))
{
	ASSERT(use_hdr);
	ASSERT(exposure);
}

void RenderWorld::update(const PostProcArg& arg)
{
	// render into msaa buffer
	{
		SCOPED_DEBUG_GROUP("rendering into msaa buffer"sv);
		auto bound = BoundFbo{msaa_buffer};
		arg.renderer->render_world(window_size, *arg.world, *arg.camera);
	}

	// copy msaa buffer to realized
	{
		SCOPED_DEBUG_GROUP("resolving msaa buffer"sv);
		resolve_multisampled_buffer(*msaa_buffer, realized_buffer.get());
	}

	// extract overexposed
	if (bloom_render.has_value())
	{
		// extract overexposed pixels
		{
			SCOPED_DEBUG_GROUP("extracting to overexposed buffer"sv);
			StateChanger{&arg.renderer->pimpl->states}
				.cull_face(false)
				.stencil_test(false)
				.depth_test(false)
				.depth_mask(false)
				.blending(false);

			glClearColor(0, 0, 0, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// todo(Gustav): why bind this late... shouldn't this be first???
			auto bound = BoundFbo{bloom_render->bloom_buffer};

			{
				const auto& container = bloom_render->extract_shader;
				const auto& shader = container->shader;
				const auto& program = shader->program;
				program->use();
				program->set_float(container->cutoff_uniform, arg.renderer->settings.bloom_cutoff);
				bind_texture_2d(&arg.renderer->pimpl->states, shader->texture_uni, *realized_buffer);

				render_geom(*arg.renderer->pimpl->full_screen_geom);
			}
		}

		// blur the buffers
		{
			SCOPED_DEBUG_GROUP("blur extracted pixels for bloom"sv);
			bool is_horizontal = true;
			for (int blur_iteration = 0; blur_iteration < arg.renderer->settings.bloom_blur_steps; blur_iteration += 1)
			{
				const auto is_first_iteration = blur_iteration == 0;

				const std::size_t source_index = is_horizontal ? 1 : 0;
				const std::size_t target_index = is_horizontal ? 0 : 1;

				SCOPED_DEBUG_GROUP(
					Str{} << "blur pass #" << blur_iteration << " ("
						<< (is_horizontal ? "hori" : "vert") << ", "
						<< (is_first_iteration ? "first" : "after") << ")"
				);
				auto bound = BoundFbo{bloom_render->ping_pong_buffer[target_index]};
				StateChanger{&arg.renderer->pimpl->states}
					.cull_face(false)
					.stencil_test(false)
					.depth_test(false)
					.depth_mask(false)
					.blending(false);

				glClearColor(0, 0, 0, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				{
					const auto& container = bloom_render->ping_pong_shader;
					const auto& shader = container->shader;
					const auto& program = shader->program;
					program->use();
					program->set_bool(container->is_horizontal_uniform, is_horizontal);

					auto& src_texture = is_first_iteration ? bloom_render->bloom_buffer
												  : bloom_render->ping_pong_buffer[source_index];
					bind_texture_2d(&arg.renderer->pimpl->states, shader->texture_uni, *src_texture);

					render_geom(*arg.renderer->pimpl->full_screen_geom);
				}

				is_horizontal = ! is_horizontal;
			}
		}
	}

	// blur overexposed using ping-pong gaussian blur
}

void RenderWorld::render(const PostProcArg& arg)
{
	// render realized (with shader)
	SCOPED_DEBUG_GROUP("render realized msaa buffer"sv);
	StateChanger{&arg.renderer->pimpl->states}
		.cull_face(false)
		.stencil_test(false)
		.depth_test(false)
		.depth_mask(false)
		.blending(false);

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ASSERT(use_hdr);
	ASSERT(exposure);

	{
		const auto& container = realize_shader;
		const auto& shader = container->shader;
		const auto& program = shader->program;
		program->use();
		program->set_float(container->gamma_uniform, arg.renderer->settings.gamma);
		program->set_float(container->exposure_uniform, *use_hdr ? *exposure : -1.0f);
		bind_texture_2d(&arg.renderer->pimpl->states, shader->texture_uni, *realized_buffer);
	
		render_geom(*arg.renderer->pimpl->full_screen_geom);
	}
}



void RenderWorld::gui()
{
	if (bloom_render && bloom_render->bloom_buffer)
	{
		imgui_image(*bloom_render->bloom_buffer);

		imgui_image(*bloom_render->ping_pong_buffer[0]);
		imgui_image(*bloom_render->ping_pong_buffer[1]);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// RenderTextureWithShader

RenderTextureWithShader::RenderTextureWithShader(std::string n, std::shared_ptr<RenderSource> s, std::shared_ptr<FrameBuffer> f, ShaderPropertyProvider* e)
	: name(std::move(n))
	, source(std::move(s))
	, fbo(std::move(f))
	, effect(e)
{
	ASSERT(effect);
}

void RenderTextureWithShader::render(const PostProcArg& arg)
{
	ASSERT(effect);

	SCOPED_DEBUG_GROUP(Str() << "Rendering task " << name);

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

void RenderTextureWithShader::update(const PostProcArg& arg)
{
	SCOPED_DEBUG_GROUP(Str() << "Updating task " << name);
	auto bound = BoundFbo{fbo};
	set_gl_viewport({fbo->width, fbo->height});
	source->render(arg);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// EffectStack

void EffectStack::update(float dt) const
{
	for (const auto& e: effects)
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
	const auto latest_msaa = arg.renderer->settings.msaa;
	if (dirty || last_msaa != latest_msaa)
	{
		dirty = false;
		last_msaa = latest_msaa;
		LOG_INFO("Building effects stack");

		compiled.targets.clear();

		auto created_world = std::make_shared<RenderWorld>(
			arg.window_size,
			&arg.renderer->pimpl->shaders_resources.pp_realize,
			&arg.renderer->pimpl->shaders_resources.pp_extract,
			&arg.renderer->pimpl->shaders_resources.pp_ping,
			latest_msaa,
			&use_hdr,
			&exposure
		);
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
	{
		SCOPED_DEBUG_GROUP("updating postproc render_world"sv);
		render_world->update(arg);
	}

	{
		SCOPED_DEBUG_GROUP("rendering postproc actions"sv);
		for (const auto& action: compiled.targets)
		{
			action->update(arg);
		}
	}

	// render the final image to the screen
	{
		SCOPED_DEBUG_GROUP("rendering postproc to screen"sv);
		set_gl_viewport(arg.window_size);
		compiled.last_source->render(arg);
	}
}

void EffectStack::gui()
{
	ImGui::Checkbox("HDR", &use_hdr);
	ImGui::SliderFloat("Exposure", &exposure, 0.01f, 20.0f);

	if (const auto rw = render_world_ref.lock(); rw)
	{
		rw->gui();
	}

	int index = 0;
	for (const auto& e: effects)
	{
		ImGui::PushID(index);
		e->gui();
		ImGui::PopID();
		index += 1;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// FactorEffect

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



///////////////////////////////////////////////////////////////////////////////////////////////////
// FloatDragShaderProp

FloatDragShaderProp::FloatDragShaderProp(const LoadedPostProcShader& shader, const std::string& n, float v, float s)
	: uniform(shader.program->get_uniform(n))
	, name(n)
	, value(v)
	, speed(s)
{
}

void FloatDragShaderProp::use(const PostProcArg&, ShaderProgram& shader)
{
	shader.set_float(uniform, value);
}

void FloatDragShaderProp::gui()
{
	ImGui::DragFloat(name.c_str(), &value, speed);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// FloatSliderShaderProp

FloatSliderShaderProp::FloatSliderShaderProp(
	const LoadedPostProcShader& shader, const std::string& n, float v, float mi, float ma
)
	: uniform(shader.program->get_uniform(n))
	, name(n)
	, value(v)
	, min(mi)
	, max(ma)
{
}

void FloatSliderShaderProp::use(const PostProcArg&, ShaderProgram& shader)
{
	shader.set_float(uniform, value);
}

void FloatSliderShaderProp::gui()
{
	ImGui::SliderFloat(name.c_str(), &value, min, max);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// SimpleEffect

SimpleEffect::SimpleEffect(std::string n, std::shared_ptr<LoadedPostProcShader> s)
	: name(std::move(n))
	, shader(std::move(s))
{
	ASSERT(shader->factor_uni.has_value());
}

void SimpleEffect::add_float_drag_prop(const std::string& prop_name, float value, float speed)
{
	properties.emplace_back(std::make_shared<FloatDragShaderProp>(*shader, prop_name, value, speed));
}

void SimpleEffect::add_float_slider_prop(const std::string& prop_name, float value, float min, float max)
{
	properties.emplace_back(std::make_shared<FloatSliderShaderProp>(*shader, prop_name, value, min, max));
}

void SimpleEffect::gui()
{
	if (properties.empty())
	{
		return;
	}

	int index = 0;

	if (ImGui::TreeNode(name.c_str()) == false)
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

	ImGui::TreePop();
}

void SimpleEffect::update(float dt)
{
	time += dt;
}

void SimpleEffect::use_shader(const PostProcArg& a, const FrameBuffer& t)
{
	shader->program->use();

	const auto& shader_factor = shader->factor_uni;
	const auto& shader_resolution = shader->resolution_uni;
	const auto& shader_time = shader->time_uni;

	if (shader_factor)
	{
		shader->program->set_float(*shader_factor, get_factor());
	}
	if (shader_resolution)
	{
		shader->program->set_vec2(*shader_resolution, a.window_size);
	}
	if (shader_time)
	{
		shader->program->set_float(*shader_time, time);
	}
	for (auto& p: properties)
	{
		p->use(a, *shader->program);
	}
	bind_texture_2d(&a.renderer->pimpl->states, shader->texture_uni, t);
}

void SimpleEffect::build(const BuildArg& arg)
{
	time = 0.0f;

	auto fbo = FrameBufferBuilder{arg.window_size}.build(USE_DEBUG_LABEL(Str() << "fbo for " << name));

	auto src = arg.builder->last_source;
	auto target = std::make_shared<RenderTextureWithShader>(name, src, fbo, this);

	arg.builder->targets.emplace_back(target);
	arg.builder->last_source = target;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// VertProvider

BlurVerticalProvider::BlurVerticalProvider(BlurEffect* b)
	: blur(b)
{
}


void BlurVerticalProvider::use_shader(const PostProcArg& a, const FrameBuffer& t)
{
	blur->use_vert_shader(a, t);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// HoriProvider

BurHorizontalProvider::BurHorizontalProvider(BlurEffect* b)
	: blur(b)
{
}

void BurHorizontalProvider::use_shader(const PostProcArg& a, const FrameBuffer& t)
{
	blur->use_hori_shader(a, t);
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// BlurEffect


BlurEffect::BlurEffect(std::string n, std::shared_ptr<LoadedPostProcShader> v, std::shared_ptr<LoadedPostProcShader> h)
	: name(std::move(n))
	, vert_p(this)
	, hori_p(this)
	, vert(std::move(v))
	, hori(std::move(h))
	, blur_size_v(vert->program->get_uniform("u_blur_size"))
	, blur_size_h(hori->program->get_uniform("u_blur_size"))
#if FF_HAS(BLUR_USE_GAUSS)
	, std_dev_v(vert->program->get_uniform("u_std_dev"))
	, std_dev_h(hori->program->get_uniform("u_std_dev"))
#endif
{
	ASSERT(vert->factor_uni.has_value());
	ASSERT(hori->factor_uni.has_value());
}

void BlurEffect::gui()
{
	if (ImGui::TreeNode(name.c_str()) == false)
	{
		return;
	}

	ImGui::SliderFloat("Blur size", &blur_size, 0.0f, 0.2f);
#if FF_HAS(BLUR_USE_GAUSS)
	ImGui::SliderFloat("Standard deviation", &std_dev, 0.0f, 0.1f);
#endif

	ImGui::TreePop();
}

void BlurEffect::update(float)
{
	// no update needed
}

void BlurEffect::use_vert_shader(const PostProcArg& a, const FrameBuffer& t) const
{
	const auto& factor_uniform = vert->factor_uni;

	vert->program->use();
	ASSERT(factor_uniform);
	if (factor_uniform)
	{
		vert->program->set_float(*factor_uniform, get_factor());
	}
	vert->program->set_float(blur_size_v, blur_size);
#if FF_HAS(BLUR_USE_GAUSS)
	vert->program->set_float(std_dev_v, std_dev);
#endif
	bind_texture_2d(&a.renderer->pimpl->states, vert->texture_uni, t);
}

void BlurEffect::use_hori_shader(const PostProcArg& a, const FrameBuffer& t)
{
	const auto& factor_uniform = hori->factor_uni;
	const auto& resolution_uniform = hori->resolution_uni;

	hori->program->use();
	ASSERT(factor_uniform);
	if (factor_uniform)
	{
		hori->program->set_float(*factor_uniform, get_factor());
	}
	ASSERT(resolution_uniform);
	if (resolution_uniform)
	{
		hori->program->set_vec2(*resolution_uniform, a.window_size);
	}
	hori->program->set_float(blur_size_h, blur_size);
#if FF_HAS(BLUR_USE_GAUSS)
	hori->program->set_float(std_dev_h, std_dev);
#endif
	bind_texture_2d(&a.renderer->pimpl->states, hori->texture_uni, t);
}

void BlurEffect::build(const BuildArg& arg)
{
	auto src = arg.builder->last_source;

	// todo(Gustav): modify resolution to get better blur and at a lower cost!

	// step 1: vertical
	auto fbo_v = FrameBufferBuilder{arg.window_size}.build(USE_DEBUG_LABEL("blur vertical"));
	auto target_v = std::make_shared<RenderTextureWithShader>("blur vertical", src, fbo_v, &vert_p);
	arg.builder->targets.emplace_back(target_v);

	// step 2: horizontal
	auto fbo_h = FrameBufferBuilder{arg.window_size}.build(USE_DEBUG_LABEL("blur horizontal"));
	auto target_h = std::make_shared<RenderTextureWithShader>("blur horizontal", target_v, fbo_h, &hori_p);
	arg.builder->targets.emplace_back(target_h);

	// done
	arg.builder->last_source = target_h;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Renderer

std::shared_ptr<FactorEffect> Renderer::make_invert_effect() const
{
	return std::make_shared<SimpleEffect>("Invert", pimpl->shaders_resources.pp_invert);
}

std::shared_ptr<FactorEffect> Renderer::make_grayscale_effect() const
{
	return std::make_shared<SimpleEffect>("Grayscale", pimpl->shaders_resources.pp_grayscale);
}

std::shared_ptr<FactorEffect> Renderer::make_damage_effect() const
{
	auto r = std::make_shared<SimpleEffect>("Damage", pimpl->shaders_resources.pp_damage);
	r->add_float_drag_prop("u_vignette_radius", 0.13f, 0.01f);
	r->add_float_slider_prop("u_vignette_smoothness", 1.0f, 0.001f, 1.0f);
	r->add_float_slider_prop("u_vignette_darkening", 1.0f, 0.0f, 1.0f);
	r->add_float_drag_prop("u_noise_scale", 25.0f, 1.0f);
	return r;
}

std::shared_ptr<FactorEffect> Renderer::make_blur_effect() const
{
	return std::make_shared<BlurEffect>("Blur", pimpl->shaders_resources.pp_blurv, pimpl->shaders_resources.pp_blurh);
}

}  //  namespace klotter
