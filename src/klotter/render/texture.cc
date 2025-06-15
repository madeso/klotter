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
	constexpr u32 FAILED_TO_LOAD_IMAGE_COLOR = 0;
	constexpr unsigned int invalid_id = 0;

	unsigned int create_texture()
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		return texture;
	}

	void set_texture_wrap(GLenum target, TextureEdge te)
	{
		const auto wrap = te == TextureEdge::clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
		glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
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
	: id(create_texture())
{
}

BaseTexture::~BaseTexture()
{
	unload();
}

BaseTexture::BaseTexture(BaseTexture&& rhs) noexcept
	: id(rhs.id)
{
	rhs.id = invalid_id;
}

BaseTexture& BaseTexture::operator=(BaseTexture&& rhs) noexcept
{
	unload();

	id = rhs.id;

	rhs.id = invalid_id;

	return *this;
}

void BaseTexture::unload()
{
	if (id != invalid_id)
	{
		glDeleteTextures(1, &id);
		id = invalid_id;
	}
}

// ------------------------------------------------------------------------------------------------
// texture 2d

Texture2d::Texture2d(const void* pixel_data, int width, int height, TextureEdge te, TextureRenderStyle trs, Transparency t)
{
	// todo(Gustav): use states
	glBindTexture(GL_TEXTURE_2D, id);

	set_texture_wrap(GL_TEXTURE_2D, te);

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
		return load_image_from_color(FAILED_TO_LOAD_IMAGE_COLOR, te, trs, t);
	}

	return {parsed.pixel_data, parsed.width, parsed.height, te, trs, t};
}

Texture2d load_image_from_color(u32 pixel, TextureEdge te, TextureRenderStyle trs, Transparency t)
{
	return {&pixel, 1, 1, te, trs, t};
}

// ------------------------------------------------------------------------------------------------
// cubemap

TextureCubemap::TextureCubemap(const std::array<void*, 6>& pixel_data, int width, int height)
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
		load_cubemap_from_color(FAILED_TO_LOAD_IMAGE_COLOR);
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
		load_cubemap_from_color(FAILED_TO_LOAD_IMAGE_COLOR);
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
		load_cubemap_from_color(FAILED_TO_LOAD_IMAGE_COLOR);
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

FrameBuffer::FrameBuffer(unsigned int f, int w, int h)
	: width(w)
	, height(h)
	, fbo(f)
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

GLenum determine_fbo_internal_format(DepthBits depth, bool add_stencil)
{
	switch (depth)
	{
	case DepthBits::use_16: return add_stencil? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT16;
	case DepthBits::use_24: return add_stencil? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24;
	case DepthBits::use_32: return add_stencil? GL_DEPTH32F_STENCIL8 : GL_DEPTH_COMPONENT32F;
	case DepthBits::use_none: return add_stencil ? GL_STENCIL_INDEX8 : GL_NONE;
	default: assert(false && "invalid enum depth value"); return GL_NONE;
	}
}

std::shared_ptr<FrameBuffer> FrameBufferBuilder::build() const
{
	const auto te = TextureEdge::clamp;
	const auto trs = TextureRenderStyle::linear;
	const auto trans = Transparency::exclude;

	const bool is_msaa = msaa_samples > 0;

	LOG_INFO("Creating frame buffer %d %d", width, height);
	auto fbo = std::make_shared<FrameBuffer>(create_fbo(), width, height);
	ASSERT(fbo->id > 0);

	fbo->debug_is_msaa = is_msaa;

	// setup texture
	const GLenum target = is_msaa ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	glBindTexture(target, fbo->id);
	if (is_msaa == false)
	{
		// msaa neither support min/mag filters nor texture wrapping
		set_texture_wrap(target, te);
		const auto filter = min_mag_from_trs(trs);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter.min);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter.mag);
	}
	const auto include_transparency = trans == Transparency::include;

	if (is_msaa)
	{
		glTexImage2DMultisample(
			target,
			msaa_samples,
			include_transparency ? GL_RGBA : GL_RGB,
			width,
			height,
			GL_TRUE
		);
	}
	else
	{
		glTexImage2D(
			target,
			0,
			include_transparency ? GL_RGBA : GL_RGB,
			width,
			height,
			0,
			include_transparency ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE,
			nullptr
		);
	}

	// setup fbo
	auto bound = BoundFbo{fbo};
	constexpr GLint mipmap_level = 0;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, fbo->id, mipmap_level);

	const auto internal_format = determine_fbo_internal_format(include_depth, include_stencil);
	if (internal_format != GL_NONE)
	{
		glGenRenderbuffers(1, &fbo->rbo);
		ASSERT(fbo->rbo != 0);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);

		if (is_msaa)
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa_samples, internal_format, width, height);
		}
		else
		{
			glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
		}

		if (include_stencil)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->rbo);
		}
	}
	else
	{
		fbo->rbo = 0;
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Failed to create frame buffer");
		return nullptr;
	}

	return fbo;
}


void resolve_multisampled_buffer(const FrameBuffer& src, FrameBuffer* dst)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src.fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->fbo);

	ASSERT(src.width == dst->width);
	ASSERT(src.height == dst->height);

	glBlitFramebuffer(0, 0, src.width, src.height, 0, 0, dst->width, dst->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


}  //  namespace klotter
