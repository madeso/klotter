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
// base texture

BaseTexture::BaseTexture()
	: id(invalid_id)
	, width(invalid_size)
	, height(invalid_size)
{
}

BaseTexture::BaseTexture(int w, int h)
	: id(create_texture())
	, width(w)
	, height(h)
{
}

BaseTexture::~BaseTexture()
{
	unload();
}

BaseTexture::BaseTexture(BaseTexture&& rhs) noexcept
	: id(rhs.id)
	, width(rhs.width)
	, height(rhs.height)
{
	rhs.id = invalid_id;
	rhs.width = invalid_size;
	rhs.height = invalid_size;
}

BaseTexture& BaseTexture::operator=(BaseTexture&& rhs) noexcept
{
	unload();

	id = rhs.id;
	width = rhs.width;
	height = rhs.height;

	rhs.id = invalid_id;
	rhs.width = invalid_size;
	rhs.height = invalid_size;

	return *this;
}

void BaseTexture::unload()
{
	if (id != invalid_id)
	{
		glDeleteTextures(1, &id);
		id = invalid_id;
	}

	width = invalid_size;
	height = invalid_size;
}

// ------------------------------------------------------------------------------------------------
// texture 2d

Texture2d::Texture2d(const void* pixel_data, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t)
	: BaseTexture(w, h)
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

struct PixelData
{
	stbi_uc* pixel_data = nullptr;
	int width = 0;
	int height = 0;

	PixelData(const embedded_binary& image_binary, bool include_transparency, bool flip = true)
	{
		int junk_channels = 0;
		stbi_set_flip_vertically_on_load(flip);

		pixel_data = stbi_load_from_memory(
			reinterpret_cast<const unsigned char*>(image_binary.data),
			Cunsigned_int_to_int(image_binary.size),
			&width,
			&height,
			&junk_channels,
			include_transparency ? 4 : 3
		);

		if (pixel_data == nullptr)
		{
			LOG_ERROR("ERROR: Failed to read pixel data");
			width = 0;
			height = 0;
		}
	}

	~PixelData()
	{
		if (pixel_data != nullptr)
		{
			stbi_image_free(pixel_data);
		}
	}

	PixelData(const PixelData&) = delete;
	PixelData(PixelData&&) = delete;
	void operator=(const PixelData&) = delete;
	void operator=(PixelData&&) = delete;
};

Texture2d load_image_from_embedded(
	const embedded_binary& image_binary, TextureEdge te, TextureRenderStyle trs, Transparency t
)
{
	const auto include_transparency = t == Transparency::include;

	auto parsed = PixelData{image_binary, include_transparency};
	if (parsed.pixel_data == nullptr)
	{
		LOG_ERROR("ERROR: Failed to load image from image source");
		return {};
	}

	return {parsed.pixel_data, parsed.width, parsed.height, te, trs, t};
}

Texture2d load_image_from_color(u32 pixel, TextureEdge te, TextureRenderStyle trs, Transparency t)
{
	return {&pixel, 1, 1, te, trs, t};
}

// ------------------------------------------------------------------------------------------------
// cubemap

TextureCubemap::TextureCubemap(const std::array<void*, 6>& pixel_data, int w, int h)
	: BaseTexture(w, h)
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

TextureCubemap load_cubemap_from_color(u32 pixel)
{
	return {{&pixel, &pixel, &pixel, &pixel, &pixel, &pixel}, 1, 1};
}

TextureCubemap load_cubemap_from_embedded(
	const embedded_binary& image_right,
	const embedded_binary& image_left,
	const embedded_binary& image_top,
	const embedded_binary& image_bottom,
	const embedded_binary& image_back,
	const embedded_binary& image_front
)
{
	constexpr auto include_transparency = false;
	constexpr auto flip = false;

	const auto parsed_right = PixelData{image_right, include_transparency, flip};
	const auto parsed_left = PixelData{image_left, include_transparency, flip};
	const auto parsed_top = PixelData{image_top, include_transparency, flip};
	const auto parsed_bottom = PixelData{image_bottom, include_transparency, flip};
	const auto parsed_back = PixelData{image_back, include_transparency, flip};
	const auto parsed_front = PixelData{image_front, include_transparency, flip};

	// is loaded ok
	if (parsed_right.pixel_data == nullptr || parsed_left.pixel_data == nullptr || parsed_top.pixel_data == nullptr
		|| parsed_bottom.pixel_data == nullptr || parsed_back.pixel_data == nullptr
		|| parsed_front.pixel_data == nullptr)
	{
		LOG_ERROR("ERROR: Failed to load some cubemap from image source");
		return {};
	}

	if (parsed_right.width == parsed_left.width && parsed_right.width == parsed_top.width
		&& parsed_right.width == parsed_bottom.width && parsed_right.width == parsed_back.width
		&& parsed_right.width == parsed_front.width)
	{
		// width ok
	}
	else
	{
		LOG_ERROR("ERROR: cubemap has inconsistent width");
		return {};
	}

	if (parsed_right.height == parsed_left.height && parsed_right.height == parsed_top.height
		&& parsed_right.height == parsed_bottom.height && parsed_right.height == parsed_back.height
		&& parsed_right.height == parsed_front.height)
	{
		// height ok
	}
	else
	{
		LOG_ERROR("ERROR: cubemap has inconsistent height");
		return {};
	}

	// ok
	return {
		{parsed_right.pixel_data,
		 parsed_left.pixel_data,
		 parsed_top.pixel_data,
		 parsed_bottom.pixel_data,
		 parsed_front.pixel_data,
		 parsed_back.pixel_data},
		parsed_right.width,
		parsed_right.height
	};
}


// ------------------------------------------------------------------------------------------------
// framebuffer


unsigned int create_fbo()
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	ASSERT(fbo != 0);
	return fbo;
}

FrameBuffer::FrameBuffer(unsigned int f)
	: fbo(f)
{
}

FrameBuffer::~FrameBuffer()
{
	if (fbo)
	{
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
	}
	if (rbo)
	{
		glDeleteRenderbuffers(1, &rbo);
		rbo = 0;
	}
}

BoundFbo::BoundFbo(std::shared_ptr<FrameBuffer> f)
	: fbo(std::move(f))
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
}

BoundFbo::~BoundFbo()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<FrameBuffer> create_frame_buffer(
	const FboSetup& set, TextureEdge te, TextureRenderStyle trs, Transparency trans
)
{
	LOG_INFO("Creating frame buffer %d %d", set.width, set.height);
	ASSERT(trs != TextureRenderStyle::mipmap);
	auto fbo = std::make_shared<FrameBuffer>(create_fbo());

	fbo->texture = Texture2d{nullptr, set.width, set.height, te, trs, trans};
	ASSERT(fbo->texture.id > 0);

	auto bound = BoundFbo{fbo};
	constexpr GLint mipmap_level = 0;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texture.id, mipmap_level);

	glGenRenderbuffers(1, &fbo->rbo);
	ASSERT(fbo->rbo != 0);
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, set.width, set.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Failed to create frame buffer");
		return nullptr;
	}

	return fbo;
}


}  //  namespace klotter
