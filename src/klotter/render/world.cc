#include <utility>

#include "klotter/render/world.h"

#include "klotter/assert.h"
#include "klotter/str.h"

#include "klotter/render/geom.extract.h"
#include "klotter/render/opengl_utils.h"
#include "klotter/render/shader.h"
#include "klotter/render/vertex_layout.h"

namespace klotter
{

CompiledGeom::CompiledGeom(u32 b, u32 a, u32 e, const CompiledGeomVertexAttributes& att, i32 tc)
	: vbo(b)
	, vao(a)
	, ebo(e)
	, number_of_triangles(tc)
	, debug_types(att.debug_types.begin(), att.debug_types.end())

{
}

CompiledGeom_TransformInstance::CompiledGeom_TransformInstance(
	u32 iv, std::size_t mi, u32 b, u32 a, u32 e, const CompiledGeomVertexAttributes& att, i32 tc
)
	: instance_vbo(iv)
	, max_instances(mi)
	, vbo(b)
	, vao(a)
	, ebo(e)
	, number_of_triangles(tc)
	, debug_types(att.debug_types.begin(), att.debug_types.end())

{
}

std::shared_ptr<MeshInstance> make_mesh_instance(std::shared_ptr<CompiledGeom> geom, std::shared_ptr<Material> mat)
{
	auto instance = std::make_shared<MeshInstance>();
	instance->geom = std::move(geom);
	instance->material = std::move(mat);
	return instance;
}

std::shared_ptr<MeshInstance_TransformInstanced> make_mesh_instance(
	std::shared_ptr<CompiledGeom_TransformInstance> geom, std::shared_ptr<Material> mat
)
{
	auto instance = std::make_shared<MeshInstance_TransformInstanced>();
	instance->geom = std::move(geom);
	instance->material = std::move(mat);
	return instance;
}

std::shared_ptr<CompiledGeom> compile_geom(DEBUG_LABEL_ARG_MANY const Geom& geom, const CompiledGeomVertexAttributes& geom_layout)
{
	const auto ex = extract_geom(geom, geom_layout);

	const auto vbo = create_buffer();
	const auto vao = create_vertex_array();
	glBindVertexArray(vao);
	SET_DEBUG_LABEL_NAMED(vao, DebugLabelFor::VertexArray, Str() << "VERT " << debug_label);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, Csizet_to_glsizeiptr(ex.data.size()), ex.data.data(), GL_STATIC_DRAW);

	const auto get_type = [](const ExtractedAttribute& extracted) -> GLenum
	{
		switch (extracted.type)
		{
		case ExtractedAttributeType::Float: return GL_FLOAT;
		default: DIE("invalid extracted attribute"); return GL_FLOAT;
		}
	};

	const auto stride = ex.stride;
	int attrib_location = 0;
	std::size_t offset = 0;
	for (const auto& att: ex.attributes)
	{
		constexpr auto normalize = false;
		glVertexAttribPointer(
			Cint_to_gluint(attrib_location),
			att.count,
			get_type(att),
			normalize ? GL_TRUE : GL_FALSE,
			Csizet_to_glsizei(stride),
			reinterpret_cast<void*>(offset)
		);
		glEnableVertexAttribArray(Cint_to_gluint(attrib_location));

		attrib_location += 1;
		offset += att.size;
	}


	const auto ebo = create_buffer();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		Csizet_to_glsizeiptr(sizeof(u32) * ex.indices.size()),
		ex.indices.data(),
		GL_STATIC_DRAW
	);

	return std::make_shared<CompiledGeom>(vbo, vao, ebo, geom_layout, ex.face_size);
}

CompiledGeom::~CompiledGeom()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	destroy_buffer(ebo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	destroy_buffer(vbo);

	glBindVertexArray(0);
	destroy_vertex_array(vao);
}

std::shared_ptr<CompiledGeom_TransformInstance> compile_geom_with_transform_instance(
	DEBUG_LABEL_ARG_MANY
	const Geom& geom, const CompiledGeomVertexAttributes& geom_layout, std::size_t max_instances
)
{
	const auto ex = extract_geom(geom, geom_layout);

	const auto instance_vbo = create_buffer();
	const auto vbo = create_buffer();
	const auto vao = create_vertex_array();
	glBindVertexArray(vao);
	SET_DEBUG_LABEL_NAMED(vao, DebugLabelFor::VertexArray, Str() << "VERT " << debug_label);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, Csizet_to_glsizeiptr(ex.data.size()), ex.data.data(), GL_STATIC_DRAW);

	const auto get_type = [](const ExtractedAttribute& extracted) -> GLenum
	{
		switch (extracted.type)
		{
		case ExtractedAttributeType::Float: return GL_FLOAT;
		default: DIE("invalid extracted attribute"); return GL_FLOAT;
		}
	};

	const auto stride = ex.stride;
	int attrib_location = 0;
	std::size_t offset = 0;
	for (const auto& att: ex.attributes)
	{
		constexpr auto normalize = false;
		glVertexAttribPointer(
			Cint_to_gluint(attrib_location),
			att.count,
			get_type(att),
			normalize ? GL_TRUE : GL_FALSE,
			Csizet_to_glsizei(stride),
			reinterpret_cast<void*>(offset)
		);
		glEnableVertexAttribArray(Cint_to_gluint(attrib_location));

		attrib_location += 1;
		offset += att.size;
	}

	// finally bind instance_vbo data, use a dummy data since the data will be uploaded before rendering
	// todo(Gustav): is dynamic draw correct?
	glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
	std::vector<glm::mat4> temp_data(max_instances);
	constexpr auto instance_size = sizeof(float) * 16;
	glBufferData(GL_ARRAY_BUFFER, Csizet_to_glsizeiptr(instance_size * max_instances), nullptr, GL_DYNAMIC_DRAW);

	for (int matrix = 0; matrix < 4; matrix += 1)
	{
		const auto attribute = Cint_to_gluint(attrib_location + matrix);
		glVertexAttribPointer(
			attribute,
			4,
			GL_FLOAT,
			GL_FALSE,
			instance_size,
			reinterpret_cast<void*>(sizeof(glm::vec4) * static_cast<std::size_t>(matrix))
		);
		glEnableVertexAttribArray(attribute);
		glVertexAttribDivisor(attribute, 1);
	}

	const auto ebo = create_buffer();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		Csizet_to_glsizeiptr(sizeof(u32) * ex.indices.size()),
		ex.indices.data(),
		GL_STATIC_DRAW
	);

	return std::make_shared<CompiledGeom_TransformInstance>(
		instance_vbo, max_instances, vbo, vao, ebo, geom_layout, ex.face_size
	);
}

LocalAxis MeshInstance::get_local_axis() const
{
	const auto m = get_mesh_rotation_matrix(rotation);
	return {
		glm::vec3{m * glm::vec4{1, 0, 0, 0}}, glm::vec3{m * glm::vec4{0, 1, 0, 0}}, glm::vec3{m * glm::vec4{0, 0, 1, 0}}
	};
}

void render_geom(const CompiledGeom& geom)
{
	ASSERT(is_bound_for_shader(geom.debug_types));
	glBindVertexArray(geom.vao);
	glDrawElements(GL_TRIANGLES, geom.number_of_triangles * 3, GL_UNSIGNED_INT, nullptr);
}

void render_geom_instanced(const MeshInstance_TransformInstanced& instanced)
{
	auto* geom = instanced.geom.get();
	ASSERT(is_bound_for_shader(geom->debug_types));
	ASSERT(! instanced.transforms.empty());

	for (std::size_t start_index = 0; start_index < instanced.transforms.size();
		 start_index += instanced.geom->max_instances)
	{
		const std::size_t step_size
			= std::min(instanced.transforms.size() - start_index, instanced.geom->max_instances);
		glBindBuffer(GL_ARRAY_BUFFER, instanced.geom->instance_vbo);
		glBufferSubData(
			GL_ARRAY_BUFFER, 0, Csizet_to_glsizeiptr(sizeof(glm::mat4) * step_size), &instanced.transforms[start_index]
		);

		glBindVertexArray(geom->vao);
		glDrawElementsInstanced(
			GL_TRIANGLES,
			geom->number_of_triangles * 3,
			GL_UNSIGNED_INT,
			nullptr,
			Csizet_to_glsizei(instanced.transforms.size())
		);
	}
}

CompiledGeom_TransformInstance::~CompiledGeom_TransformInstance()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	destroy_buffer(ebo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	destroy_buffer(vbo);
	destroy_buffer(instance_vbo);

	glBindVertexArray(0);
	destroy_vertex_array(vao);
}


}  //  namespace klotter
