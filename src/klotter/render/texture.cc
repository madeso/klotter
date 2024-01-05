#include "klotter/render/texture.h"

#include "stb_image.h"

#include "klotter/assert.h"
#include "klotter/cint.h"
#include "klotter/log.h"

#include "klotter/render/opengl_utils.h"
#include "klotter/render/texture.io.h"

namespace klotter
{


// ------------------------------------------------------------------------------------------------
// general

namespace
{
	constexpr unsigned int invalid_id = 0;
	constexpr int invalid_size = -1;

	unsigned int create_texture()
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		return texture;
	}

	void set_texture_wrap(TextureEdge te)
	{
		const auto wrap = te == TextureEdge::clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	}

	struct MinMagFilter
	{
		GLint min;
		GLint mag;
	};

	MinMagFilter min_mag_from_trs(TextureRenderStyle trs)
	{
		switch (trs)
		{
		case TextureRenderStyle::pixel: return {GL_NEAREST, GL_NEAREST};
		case TextureRenderStyle::linear: return {GL_LINEAR, GL_LINEAR};
		case TextureRenderStyle::mipmap: return {GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR};
		default: DIE("Invalid texture render style"); return {GL_NEAREST, GL_NEAREST};
		}
	}
}  //  namespace

// ------------------------------------------------------------------------------------------------
// texture

Texture::Texture()
	: id(invalid_id)
	, width(invalid_size)
	, height(invalid_size)
{
}

Texture::Texture(
	void* pixel_data, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t
)
	: id(create_texture())
	, width(w)
	, height(h)
{
	// todo(Gustav): use states
	glBindTexture(GL_TEXTURE_2D, id);

	set_texture_wrap(te);

	const auto filter = min_mag_from_trs(trs);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter.min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter.mag);

	const auto include_transparency = t == Transparency::include;

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		include_transparency ? GL_RGBA : GL_RGB,
		width,
		height,
		0,
		include_transparency ? GL_RGBA : GL_RGB,
		GL_UNSIGNED_BYTE,
		pixel_data
	);

	if (trs == TextureRenderStyle::mipmap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}

Texture::~Texture()
{
	unload();
}

Texture::Texture(Texture&& rhs)
	: id(rhs.id)
	, width(rhs.width)
	, height(rhs.height)
{
	rhs.id = invalid_id;
	rhs.width = invalid_size;
	rhs.height = invalid_size;
}

void Texture::operator=(Texture&& rhs)
{
	unload();

	id = rhs.id;
	width = rhs.width;
	height = rhs.height;

	rhs.id = invalid_id;
	rhs.width = invalid_size;
	rhs.height = invalid_size;
}

void Texture::unload()
{
	if (id != invalid_id)
	{
		glDeleteTextures(1, &id);
		id = invalid_id;
	}

	width = invalid_size;
	height = invalid_size;
}

Texture LoadImage(
	const unsigned char* image_source,
	int size,
	TextureEdge te,
	TextureRenderStyle trs,
	Transparency t
)
{
	const auto include_transparency = t == Transparency::include;

	int width = 0;
	int height = 0;
	int junk_channels = 0;

	stbi_set_flip_vertically_on_load(true);
	auto* pixel_data = stbi_load_from_memory(
		image_source, size, &width, &height, &junk_channels, include_transparency ? 4 : 3
	);

	if (pixel_data == nullptr)
	{
		LOG_ERROR("ERROR: Failed to load image from image source");
		return {};
	}

	auto loaded = Texture{pixel_data, width, height, te, trs, t};

	stbi_image_free(pixel_data);

	return loaded;
}

Texture load_image_from_embedded(
	const embedded_binary& image_binary, TextureEdge te, TextureRenderStyle trs, Transparency t
)
{
	return LoadImage(
		reinterpret_cast<const unsigned char*>(image_binary.data),
		Cunsigned_int_to_int(image_binary.size),
		te,
		trs,
		t
	);
}

Texture load_image_from_color(u32 pixel, TextureEdge te, TextureRenderStyle trs, Transparency t)
{
	return {&pixel, 1, 1, te, trs, t};
}

// ------------------------------------------------------------------------------------------------
// cubemap

Cubemap::Cubemap()
	: id(invalid_id)
	, width(invalid_size)
	, height(invalid_size)
{
}

Cubemap::Cubemap(std::array<void*, 6> pixel_data, int w, int h)
	: id(create_texture())
	, width(w)
	, height(h)
{
	// todo(Gustav): use states
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	for (size_t index = 0; index < 6; index += 1)
	{
		glTexImage2D(
			static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index),
			0,
			GL_RGB,
			width,
			height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			pixel_data[index]
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Cubemap::~Cubemap()
{
	unload();
}

Cubemap::Cubemap(Cubemap&& rhs)
	: id(rhs.id)
	, width(rhs.width)
	, height(rhs.height)
{
	rhs.id = invalid_id;
	rhs.width = invalid_size;
	rhs.height = invalid_size;
}

void Cubemap::operator=(Cubemap&& rhs)
{
	unload();

	id = rhs.id;
	width = rhs.width;
	height = rhs.height;

	rhs.id = invalid_id;
	rhs.width = invalid_size;
	rhs.height = invalid_size;
}

void Cubemap::unload()
{
	if (id != invalid_id)
	{
		glDeleteTextures(1, &id);
		id = invalid_id;
	}

	width = invalid_size;
	height = invalid_size;
}

Cubemap load_cubemap_from_color(u32 pixel)
{
	return {{&pixel, &pixel, &pixel, &pixel, &pixel, &pixel}, 1, 1};
}


}  //  namespace klotter
