#pragma once

#include <memory>
#include <optional>

#include "klotter/colors.h"
#include "klotter/render/texture.h"

namespace klotter
{
struct Assets;
struct CompiledCamera;
struct LoadedShader_Default;
struct LoadedShader_Unlit;
struct RenderSettings;
struct Lights;
struct OpenglStates;

/** \addtogroup render Renderer
 *  @{
*/

/// All loaded/known shaders
struct ShaderResource;
struct RenderContext;

/// Base class for all materials
struct Material
{
	Material() = default;
	virtual ~Material() = default;

	Material(const Material&) = delete;
	Material(Material&&) = delete;
	void operator=(const Material&) = delete;
	void operator=(Material&&) = delete;

	virtual void use_shader(const RenderContext&) = 0;
	virtual void set_uniforms(const RenderContext&, const CompiledCamera&, const std::optional<glm::mat4>&) = 0;
	virtual void bind_textures(const RenderContext&, OpenglStates* states, Assets* assets) = 0;
	virtual void apply_lights(
		const RenderContext&, const Lights& lights, const RenderSettings& settings, OpenglStates* states, Assets* assets
	) = 0;

	[[nodiscard]] virtual bool is_transparent() const = 0;
};

/// A unlit (or fully lit) material, not affected by light.
struct UnlitMaterial : Material
{
	const LoadedShader_Unlit* shader;
	glm::vec3 color = colors::white;
	float alpha = 1.0f;
	std::shared_ptr<Texture2d> texture;

	explicit UnlitMaterial(const ShaderResource& resource);
	void use_shader(const RenderContext&) override;
	void set_uniforms(const RenderContext&, const CompiledCamera&, const std::optional<glm::mat4>&) override;
	void bind_textures(const RenderContext&, OpenglStates* states, Assets* assets) override;
	void apply_lights(
		const RenderContext&, const Lights& lights, const RenderSettings& settings, OpenglStates* states, Assets* assets
	) override;

	[[nodiscard]] bool is_transparent() const override;
};

/// A material affected by light.
struct DefaultMaterial : Material
{
	const LoadedShader_Default* shader = nullptr;
	glm::vec3 color = colors::white;
	float alpha = 1.0f;

	glm::vec3 ambient_tint = colors::white;
	glm::vec3 specular_color = colors::white;
	float shininess = 32.0f;
	float emissive_factor = 0.0f;

	std::shared_ptr<Texture2d> diffuse;
	std::shared_ptr<Texture2d> specular;
	std::shared_ptr<Texture2d> emissive;

	explicit DefaultMaterial(const ShaderResource& resource);
	void use_shader(const RenderContext&) override;
	void set_uniforms(const RenderContext&, const CompiledCamera&, const std::optional<glm::mat4>&) override;
	void bind_textures(const RenderContext&, OpenglStates* states, Assets* assets) override;
	void apply_lights(
		const RenderContext&, const Lights& lights, const RenderSettings& settings, OpenglStates* states, Assets* assets
	) override;

	[[nodiscard]] bool is_transparent() const override;
};

/**
 * @}
*/

}  //  namespace klotter
