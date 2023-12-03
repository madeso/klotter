#include "klotter/render/shader.source.h"

#include "klotter/log.h"
#include "klotter/str.h"

#include "mustache/mustache.hpp"

#include "default_shader.vert.glsl.h"
#include "default_shader.frag.glsl.h"

namespace klotter
{


using Properties = std::unordered_map<std::string, std::string>;

std::string generate(std::string_view str, const ShaderOptions& options)
{
	auto input = kainjow::mustache::mustache{std::string{str.begin(), str.end()}};
	if (input.is_valid() == false)
	{
		const auto error = input.error_message();
		LOG_ERROR("Failed to parse mustache: %s", error.c_str());
	}

	input.set_custom_escape([](const std::string& s) { return s; });
	auto data = kainjow::mustache::data{};

	data["use_lights"] = options.use_lights;
	data["number_of_point_lights"] = (Str() << options.number_of_point_lights).str();
	data["number_of_directional_lights"] = (Str() << options.number_of_directional_lights).str();
	return input.render(data);
}

ShaderSource load_shader_source(const ShaderOptions& options)
{
	auto layout = ShaderVertexAttributes{
		{VertexType::position3, "a_position"},
		{VertexType::color3, "a_color"},
		{VertexType::texture2, "a_tex_coord"}};

	if (options.use_lights)
	{
		layout.emplace_back(VertexElementDescription{VertexType::normal3, "a_normal"});
	}

	return ShaderSource{
		layout,
		generate(DEFAULT_SHADER_VERT_GLSL, options),
		generate(DEFAULT_SHADER_FRAG_GLSL, options)};
}

}  //  namespace klotter
