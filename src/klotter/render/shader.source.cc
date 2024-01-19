#include "klotter/render/shader.source.h"

#include "klotter/log.h"
#include "klotter/str.h"

#include "mustache/mustache.hpp"

#include "default_shader.vert.glsl.h"
#include "default_shader.frag.glsl.h"

namespace klotter
{


using Properties = std::unordered_map<std::string, std::string>;

ShaderOptions ShaderOptions::with_transparent_cutoff() const
{
	auto ret = *this;
	ret.transparent_cutoff = true;
	return ret;
}

kainjow::mustache::mustache load_mustache(std::string_view str)
{
	auto input = kainjow::mustache::mustache{std::string{str.begin(), str.end()}};
	if (input.is_valid() == false)
	{
		const auto& error = input.error_message();
		LOG_ERROR("Failed to parse mustache: %s", error.c_str());
	}

	input.set_custom_escape([](const std::string& s) { return s; });
	return input;
}

std::string generate_blur(std::string_view src, const BlurOptions& options)
{
	auto input = load_mustache(src);
	auto data = kainjow::mustache::data{};

	data["is_horizontal"] = options.blur == BlurType::horizontal;
	data["is_vertical"] = options.blur == BlurType::vertical;
	data["sample_count"] = (Str{} << options.sample_count).str();
	data["is_gauss"] = options.is_gauss == IsGauss::yes;

	return input.render(data);
}

std::string generate(
	std::string_view str, const ShaderOptions& options, const std::string& uniform_buffer_source
)
{
	auto input = load_mustache(str);
	auto data = kainjow::mustache::data{};

	data["use_lights"] = options.use_lights;
	data["use_texture"] = options.use_texture;
	data["number_of_directional_lights"] = (Str() << options.number_of_directional_lights).str();
	data["number_of_point_lights"] = (Str() << options.number_of_point_lights).str();
	data["number_of_frustum_lights"] = (Str() << options.number_of_frustum_lights).str();
	data["transparent_cutoff"] = options.transparent_cutoff;
	data["uniform_buffer_source"] = uniform_buffer_source;

	return input.render(data);
}

VertexShaderSource load_shader_source(
	const ShaderOptions& options, const std::string& uniform_buffer_source
)
{
	auto layout = ShaderVertexAttributes{
		{VertexType::position3, "a_position"}, {VertexType::color3, "a_color"}};

	if (options.use_texture)
	{
		layout.emplace_back(VertexElementDescription{VertexType::texture2, "a_tex_coord"});
	}

	if (options.use_lights)
	{
		layout.emplace_back(VertexElementDescription{VertexType::normal3, "a_normal"});
	}

	return VertexShaderSource{
		layout,
		generate(DEFAULT_SHADER_VERT_GLSL, options, uniform_buffer_source),
		generate(DEFAULT_SHADER_FRAG_GLSL, options, uniform_buffer_source)};
}

}  //  namespace klotter
