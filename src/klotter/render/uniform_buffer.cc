#include "klotter/render/uniform_buffer.h"

#include "klotter/assert.h"

namespace klotter
{

using UniformBufferDescription = std::vector<UniformProp>;

/// returns the size in bytes
int size_of(UniformProp prop, bool align)
{
	constexpr int n = 4;  // in bytes
	constexpr int n2 = n * 2;
	constexpr int n4 = n * 4;
	switch (prop.array_count == 1 ? prop.type : UniformType::vec4)
	{
	case UniformType::bool_type: return n;
	case UniformType::int_type: return n;
	case UniformType::float_type: return n;
	case UniformType::vec2: return n2;
	case UniformType::vec3: return n4;
	case UniformType::vec4: return n4;
	case UniformType::mat4: return n4 * (align ? 1 : 4);
	default: DIE("invalid uniform type"); return 0;
	}
}

int calculate_alignment_bytes(int current_size, UniformProp prop)
{
	const auto size = size_of(prop, true);
	const auto mod = current_size % size;
	if (mod == 0) return 0;
	const auto to_add = size - mod;
	ASSERT((to_add + mod) % size == 0);
	return to_add;
}

std::string to_source(const std::string& name, const UniformBufferDescription& desc)
{
	const auto type_to_string = [](UniformType t)
	{
		switch (t)
		{
		case UniformType::bool_type: return "bool";
		case UniformType::int_type: return "int";
		case UniformType::float_type: return "float";
		case UniformType::vec2: return "vec2";
		case UniformType::vec3: return "vec3";
		case UniformType::vec4: return "vec4";
		case UniformType::mat4: return "mat4";
		default: DIE("invalid uniform type"); return "unhandled_type";
		}
	};
	std::ostringstream ss;
	ss << "layout (std140) uniform " << name << "\n";
	ss << "{"
	   << "\n";
	for (const auto& p: desc)
	{
		ss << "\t" << type_to_string(p.type) << " " << p.name;
		if (p.array_count > 1)
		{
			ss << "[" << p.array_count << "]";
		}
		ss << ";\n";
	}
	ss << "};"
	   << "\n";
	return ss.str();
}

void UniformBufferCompiler::add(
	CompiledUniformProp* target, UniformType type, const std::string& name, int array_count
)
{
	props.emplace_back(UniformProp{target, type, name, array_count});
}

void UniformBufferCompiler::compile(
	const std::string& name, UniformBufferSetup* target, int binding_point
)
{
	target->size = 0;
	target->binding_point = binding_point;
	target->name = name;
	target->source = to_source(name, props);
	for (const auto& p: props)
	{
		const auto size = size_of(p, false);
		target->size += calculate_alignment_bytes(target->size, p);
		*p.target = {target->size, p.type, p.array_count};
		target->size += size * p.array_count;
	}
}

namespace
{
	UniformBuffer* bound_buffer = nullptr;
}  //  namespace

BoundUniformBuffer::BoundUniformBuffer(UniformBuffer* b)
	: buffer(b)
{
	ASSERT(bound_buffer == nullptr);
	bound_buffer = b;
	glBindBuffer(GL_UNIFORM_BUFFER, b->id);
}

BoundUniformBuffer::~BoundUniformBuffer()
{
	ASSERT(bound_buffer == buffer);
	bound_buffer = nullptr;
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::UniformBuffer(const UniformBufferSetup& setup)
{
	// todo(Gustav): replace all buffer creation/destruction with 2 basic functions

	glGenBuffers(1, &id);
	auto bound = BoundUniformBuffer{this};
	glBufferData(GL_UNIFORM_BUFFER, setup.size, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, setup.binding_point, id);
}

UniformBuffer::~UniformBuffer()
{
	unload();
}

UniformBuffer::UniformBuffer(UniformBuffer&& rhs) noexcept
	: id(rhs.id)
{
	rhs.id = 0;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& rhs) noexcept
{
	unload();
	id = rhs.id;
	rhs.id = 0;
	return *this;
}

// clears the loaded buffer to a invalid buffer
void UniformBuffer::unload()
{
	if (id == 0)
	{
		return;
	}

	glDeleteBuffers(1, &id);
	id = 0;
}

void UniformBuffer::set_mat4(const CompiledUniformProp& prop, const glm::mat4& m)
{
	// todo(Gustav): verify that the prop belongs to self
	ASSERT(prop.type == UniformType::mat4 && prop.array_count == 1);
	// todo(Gustav): replace sizeof with actual size
	glBufferSubData(GL_UNIFORM_BUFFER, prop.offset, sizeof(glm::mat4), glm::value_ptr(m));
}

}  //  namespace klotter
