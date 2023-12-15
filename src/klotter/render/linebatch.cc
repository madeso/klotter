#include "linebatch.h"

#include "klotter/assert.h"
#include "klotter/dependency_glad.h"

namespace klotter
{
DebugDrawer::DebugDrawer()
	// todo(Gustav): exapnd shader with stipple pattern https://stackoverflow.com/questions/52928678/dashed-line-in-opengl3/54543267#54543267
	// todo(Gustav): move quad_description and quad_layout to a seperate setup
	: line_description({{VertexType::position3, "a_position"}, {VertexType::color3, "a_color"}})
	, line_layout(
		  compile_shader_layout(compile_attribute_layouts({line_description}), line_description)
	  )
	, line_shader(
		  R"glsl(
            #version 430 core
            in vec3 a_position;
            in vec3 a_color;

            uniform mat4 u_projection;
			uniform mat4 u_view;

            out vec4 v_color;

            void main()
            {
                v_color = vec4(a_color, 1.0);
                gl_Position = u_projection * u_view * vec4(a_position.xyz, 1.0);
            }
        )glsl",
		  R"glsl(
            #version 430 core

            in vec4 v_color;

            out vec4 color;

            void main()
            {
                color = v_color;
            }
        )glsl",
		  line_layout
	  )
	, line_projection(line_shader.get_uniform("u_projection"))
	, line_view(line_shader.get_uniform("u_view"))
	, line_batch(&line_shader)
{
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

	constexpr auto vertex_size = 6 * sizeof(float);
	constexpr auto max_vertices = 2 * max_lines;
	constexpr auto max_indices = 2 * max_lines;

	glGenBuffers(1, &vb);
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

	std::vector<u32> indices;
	indices.reserve(max_indices);

	for (auto quad_index = 0; quad_index < max_lines; quad_index += 1)
	{
		const auto base = quad_index * 2;
		indices.emplace_back(base + 0);
		indices.emplace_back(base + 1);
	}

	ASSERT(max_indices == indices.size());

	glGenBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, max_indices * sizeof(u32), indices.data(), GL_STATIC_DRAW
	);
}

LineBatch::~LineBatch()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ib);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vb);

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
		GL_ARRAY_BUFFER,
		0,
		static_cast<GLsizeiptr>(sizeof(float) * data.size()),
		static_cast<const void*>(data.data())
	);
	glDrawElements(GL_LINES, 2 * lines, GL_UNSIGNED_INT, nullptr);

	data.resize(0);
	lines = 0;
}

}  //  namespace klotter
