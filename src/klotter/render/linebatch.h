#pragma once

#include "klotter/render/shader.h"

namespace klotter
{

/// Helper to batch lines for rendering.
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

	void line(const glm::vec3& world_from, const glm::vec3& world_to, const glm::vec3& color);
	void submit();
};

// todo(Gustav): merge with LineBatch?
/// A utility to draw 3d lines
struct LineDrawer
{
	ShaderVertexAttributes description;
	CompiledShaderVertexAttributes layout;
	ShaderProgram shader;
	Uniform clip_from_view_uni;
	Uniform view_from_world_uni;
	Uniform resolution_uni;
	Uniform dash_size_uni;
	Uniform gap_size_uni;
	LineBatch line_batch;

	LineDrawer();

	void set_line_to_dash(const glm::vec2& resolution, float dash_size, float gap_size);
	void set_line_to_solid();

	[[nodiscard]] bool is_loaded() const;
};

}  //  namespace klotter
