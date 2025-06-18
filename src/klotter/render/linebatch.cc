#include "linebatch.h"

#include "klotter/assert.h"
#include "klotter/dependency_glad.h"

#include "klotter/render/opengl_utils.h"

namespace klotter
{
DebugDrawer::DebugDrawer()
	// todo(Gustav): exapnd shader with stipple pattern https://stackoverflow.com/questions/52928678/dashed-line-in-opengl3/54543267#54543267
	// todo(Gustav): move quad_description and quad_layout to a seperate setup
	: line_description({{VertexType::position3, "a_position"}, {VertexType::color3, "a_color"}})
	, line_layout(compile_shader_layout(compile_attribute_layouts({line_description}), line_description, std::nullopt))
	, line_shader(
		  USE_PROGRAM_LABEL("debug line")
		  R"glsl(
			#version 430 core
			in vec3 a_position;
			in vec3 a_color;

			uniform mat4 u_projection;
			uniform mat4 u_view;
			uniform vec2  u_resolution;

			out vec4 v_color;
			out vec2 v_vert_pos;
			flat out vec2 v_start_pos;

			void main()
			{
				v_color = vec4(a_color, 1.0);
				vec4 pos = u_projection * u_view * vec4(a_position.xyz, 1.0);
				gl_Position = pos;
				v_vert_pos = ((pos.xy / pos.w) + 1)/2.0 * u_resolution;
				v_start_pos = v_vert_pos;
			}
		)glsl",
		  R"glsl(
			#version 430 core

			in vec4 v_color;
			flat in vec2 v_start_pos;
			in vec2 v_vert_pos;

			uniform float u_dash_size;
			uniform float u_gap_size;
			uniform vec2  u_resolution;

			out vec4 color;

			void main()
			{
				if(u_resolution.x > 0.0)
				{
					float seg_len = u_dash_size + u_gap_size;

					float dash_seg = u_dash_size/seg_len;

					float cur = length(v_vert_pos - v_start_pos);
					float cur_seg = fract(cur / seg_len);
					if(cur_seg > dash_seg)
					{
						discard;
					}
					color = v_color;
				}
				else
				{
					color = v_color;
				}
			}
		)glsl",
		  line_layout
	  )
	, line_projection(line_shader.get_uniform("u_projection"))
	, line_view(line_shader.get_uniform("u_view"))
	, line_resolution(line_shader.get_uniform("u_resolution"))
	, line_dash_size(line_shader.get_uniform("u_dash_size"))
	, line_gap_size(line_shader.get_uniform("u_gap_size"))
	, line_batch(&line_shader)
{
}

void DebugDrawer::set_line_to_dash(const glm::vec2& resolution, float dash_size, float gap_size)
{
	line_shader.set_vec2(line_resolution, resolution);
	line_shader.set_float(line_dash_size, dash_size);
	line_shader.set_float(line_gap_size, gap_size);
}

void DebugDrawer::set_line_to_solid()
{
	line_shader.set_vec2(line_resolution, {-1.0f, -1.0f});
}

bool DebugDrawer::is_loaded() const
{
	return line_shader.is_loaded();
}

LineBatch::LineBatch(ShaderProgram* shader)
{
	shader->use();

	glGenVertexArrays(1, &va);
	glBindVertexArray(va);

	constexpr auto vertex_count = 2;
	constexpr auto float_per_vertex = 3;
	constexpr auto float_count = vertex_count * float_per_vertex;
	constexpr auto vertex_size = float_count * sizeof(float);
	constexpr auto max_vertices = vertex_count * max_lines;
	constexpr auto max_indices = vertex_count * max_lines;

	vb = create_buffer();
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, vertex_size * max_vertices, nullptr, GL_DYNAMIC_DRAW);

	auto relative_offset = [](unsigned int i)
	{
		return reinterpret_cast<void*>(i * sizeof(float));
	};

	unsigned int offset = 0;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
	offset += 3;

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
	offset += 3;
	ASSERT(offset == float_count);

	std::vector<u32> indices;
	indices.reserve(max_indices);

	for (auto quad_index = 0; quad_index < max_lines; quad_index += 1)
	{
		const auto base = quad_index * 2;
		indices.emplace_back(base + 0);
		indices.emplace_back(base + 1);
	}

	ASSERT(max_indices == indices.size());

	ib = create_buffer();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices * sizeof(u32), indices.data(), GL_STATIC_DRAW);
}

LineBatch::~LineBatch()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	destroy_buffer(ib);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	destroy_buffer(vb);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &va);
}

void add_vertex(LineBatch* batch, const glm::vec3& position, const glm::vec3& color)
{
	batch->data.push_back(position.x);
	batch->data.push_back(position.y);
	batch->data.push_back(position.z);

	batch->data.push_back(color.x);
	batch->data.push_back(color.y);
	batch->data.push_back(color.z);
}

void LineBatch::line(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color)
{
	if (lines == max_lines)
	{
		submit();
	}

	lines += 1;

	add_vertex(this, from, color);
	add_vertex(this, to, color);
}

void LineBatch::submit()
{
	if (lines == 0)
	{
		return;
	}

	// this assumes set_model_projection_view has been called

	glBindVertexArray(va);

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferSubData(
		GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(sizeof(float) * data.size()), static_cast<const void*>(data.data())
	);
	glDrawElements(GL_LINES, 2 * lines, GL_UNSIGNED_INT, nullptr);

	data.resize(0);
	lines = 0;
}

}  //  namespace klotter
