#pragma once

#include "klotter/render/shader.h"

namespace klotter
{

struct LineBatch
{
	static constexpr int max_lines = 100;

	std::vector<float> data;
	int lines = 0;
	u32 va;
	u32 vb;
	u32 ib;

	explicit LineBatch(ShaderProgram*);
	~LineBatch();

	LineBatch(const LineBatch&) = delete;
	void operator=(const LineBatch&) = delete;
	LineBatch(LineBatch&&) = delete;
	void operator=(LineBatch&&) = delete;

	void line(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);
	void submit();
};

struct DebugDrawer
{
	ShaderVertexAttributes line_description;
	CompiledShaderVertexAttributes line_layout;
	ShaderProgram line_shader;
	Uniform line_projection;
	Uniform line_view;
	Uniform line_resolution;
	Uniform line_dash_size;
	Uniform line_gap_size;
	LineBatch line_batch;

	DebugDrawer();

	void set_line_dash(const glm::vec2& resolution, float dash_size, float gap_size);
	void set_line_line();

	[[nodiscard]] bool is_loaded() const;
};

}  //  namespace klotter
