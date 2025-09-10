#include "klotter/render/texture.h"

#include "klotter/assert.h"
#include "klotter/cint.h"
#include "klotter/log.h"
#include "klotter/str.h"
#include "klotter/cpp.h"

#include "klotter/render/opengl_utils.h"
#include "klotter/render/texture.io.h"

#include "stb_image.h"


namespace klotter
{


// ------------------------------------------------------------------------------------------------
// general

namespace
{
	/// the color of an image that failed to load, html hot-pink
	constexpr SingleColor image_load_failure_color = color_from_rgba(0xFF, 0x69, 0xB4, 0xFF);

	constexpr unsigned int invalid_id = 0;

	[[nodiscard]]
	unsigned int create_texture()
	{
		unsigned int texture = 0;
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

	[[nodiscard]]
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

	[[nodiscard]]
	GLint internal_format_from_color_data(Transparency t, ColorData cd)
	{
		const auto include_transparency = t == Transparency::include;
		switch(cd)
		{
		case ColorData::color_data: return include_transparency ? GL_SRGB_ALPHA : GL_SRGB;
		case ColorData::dont_care:
		case ColorData::non_color_data: return include_transparency ? GL_RGBA : GL_RGB;
		default:
			DIE("Invalid color data type");
			return GL_RGBA;
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

Texture2d::Texture2d(DEBUG_LABEL_ARG_MANY const void* pixel_data, unsigned int pixel_format, int width, int height, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd)
{
	// todo(Gustav): use states
	glBindTexture(GL_TEXTURE_2D, id);
	SET_DEBUG_LABEL_NAMED(id, DebugLabelFor::Texture, Str() << "TEXTURE2d " << debug_label);

	set_texture_wrap(GL_TEXTURE_2D, te);

	const auto filter = min_mag_from_trs(trs);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter.min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter.mag);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		internal_format_from_color_data(t, cd),
		width,
		height,
		0,
		pixel_format,
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
		stbi_set_flip_vertically_on_load(flip ? 1 : 0);

		pixel_data = stbi_load_from_memory(
			reinterpret_cast<const unsigned char*>(image_binary.data),
			int_from_unsigned_int(image_binary.size),
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

[[nodiscard]]
Texture2d load_image_from_embedded(
	DEBUG_LABEL_ARG_MANY const embedded_binary& image_binary, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd
)
{
	const auto include_transparency = t == Transparency::include;

	auto parsed = PixelData{image_binary, include_transparency};
	if (parsed.pixel_data == nullptr)
	{
		LOG_ERROR("ERROR: Failed to load image from image source");
		return load_image_from_color(SEND_DEBUG_LABEL_MANY(debug_label) image_load_failure_color, te, trs, t, cd);
	}

	const GLenum pixel_format = include_transparency ? GL_RGBA : GL_RGB;
	return {SEND_DEBUG_LABEL_MANY(debug_label) parsed.pixel_data, pixel_format, parsed.width, parsed.height, te, trs, t, cd};
}

[[nodiscard]]
Texture2d load_image_from_color(DEBUG_LABEL_ARG_MANY SingleColor pixel, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd)
{
	return {SEND_DEBUG_LABEL_MANY(debug_label) & pixel, GL_RGBA, 1, 1, te, trs, t, cd};
}

// ------------------------------------------------------------------------------------------------
// cubemap

TextureCubemap::TextureCubemap(DEBUG_LABEL_ARG_MANY const std::array<void*, 6>& pixel_data, int width, int height, ColorData cd)
{
	// todo(Gustav): use states
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	SET_DEBUG_LABEL_NAMED(id, DebugLabelFor::Texture, Str() << "TEXTURE CUBEMAP " << debug_label);

	for (size_t index = 0; index < 6; index += 1)
	{
		glTexImage2D(
			static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index),
			0,
			internal_format_from_color_data(Transparency::exclude, cd),
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

[[nodiscard]]
TextureCubemap load_cubemap_from_color(DEBUG_LABEL_ARG_MANY SingleColor pixel, ColorData cd)
{
	return {SEND_DEBUG_LABEL_MANY(debug_label) {&pixel, &pixel, &pixel, &pixel, &pixel, &pixel}, 1, 1, cd};
}

// right, left, top, bottom, back, front,
[[nodiscard]]
TextureCubemap load_cubemap_from_embedded(
	DEBUG_LABEL_ARG_MANY
	const std::array<embedded_binary, 6>& images,
	ColorData cd
)
{
	constexpr auto include_transparency = false;
	constexpr auto flip = false;

	const auto parsed_right = PixelData{images[0], include_transparency, flip};
	const auto parsed_left = PixelData{images[1], include_transparency, flip};
	const auto parsed_top = PixelData{images[2], include_transparency, flip};
	const auto parsed_bottom = PixelData{images[3], include_transparency, flip};
	const auto parsed_back = PixelData{images[4], include_transparency, flip};
	const auto parsed_front = PixelData{images[5], include_transparency, flip};

	// is loaded ok
	if (any_is_null(std::array{
		parsed_right.pixel_data, parsed_left.pixel_data,
		parsed_top.pixel_data, parsed_bottom.pixel_data,
		parsed_back.pixel_data, parsed_front.pixel_data}))
	{
		LOG_ERROR("ERROR: Failed to load some cubemap from image source");
		return load_cubemap_from_color(SEND_DEBUG_LABEL_MANY(debug_label) image_load_failure_color, cd);
	}

	if (all_equal(std::array{
		parsed_right.width, parsed_left.width,
		parsed_top.width, parsed_bottom.width,
		parsed_back.width, parsed_front.width}))
	{
		// width ok
	}
	else
	{
		LOG_ERROR("ERROR: cubemap has inconsistent width");
		return load_cubemap_from_color(SEND_DEBUG_LABEL_MANY(debug_label) image_load_failure_color, cd);
	}

	if (all_equal(std::array{
		parsed_right.height, parsed_left.height,
		parsed_top.height, parsed_bottom.height,
		parsed_back.height, parsed_front.height}))
	{
		// height ok
	}
	else
	{
		LOG_ERROR("ERROR: cubemap has inconsistent height");
		return load_cubemap_from_color(SEND_DEBUG_LABEL_MANY(debug_label) image_load_failure_color, cd);
	}

	// ok
	return {
		SEND_DEBUG_LABEL_MANY(debug_label)
		{parsed_right.pixel_data,
		 parsed_left.pixel_data,
		 parsed_top.pixel_data,
		 parsed_bottom.pixel_data,
		 parsed_front.pixel_data,
		 parsed_back.pixel_data},
		parsed_right.width,
		parsed_right.height,
		cd
	};
}


// ------------------------------------------------------------------------------------------------
// framebuffer

[[nodiscard]]
unsigned int create_fbo()
{
	unsigned int fbo = 0;
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
	if (fbo != 0)
	{
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
	}
	if (rbo != 0)
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

[[nodiscard]]
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

// this is templated because opengl can't decide if the internfal format is a GLint or GLenum
template<typename R>
R internal_format_from_color_bpp(ColorBitsPerPixel texture_bits, Transparency trans)
{
	const auto include_transparency = trans == Transparency::include;

	switch (texture_bits)
	{
	case ColorBitsPerPixel::use_8: return include_transparency ? GL_RGBA : GL_RGB;
	case ColorBitsPerPixel::use_16: return include_transparency ? GL_RGBA16F : GL_RGB16F;
	case ColorBitsPerPixel::use_32: return include_transparency ? GL_RGBA32F : GL_RGB32F;
	default:
		DIE("Invalid texture bits value");
		return GL_RGB;
	}
}

std::shared_ptr<FrameBuffer> FrameBufferBuilder::build(DEBUG_LABEL_ARG_SINGLE) const
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
	SET_DEBUG_LABEL_NAMED(fbo->id, DebugLabelFor::Texture, Str() << "TEXTURE FRAMEBUFFER " << debug_label);
	if (is_msaa == false)
	{
		// msaa neither support min/mag filters nor texture wrapping
		set_texture_wrap(target, te);
		const auto filter = min_mag_from_trs(trs);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter.min);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter.mag);
	}

	if (is_msaa)
	{
		glTexImage2DMultisample(
			target,
			msaa_samples,
			internal_format_from_color_bpp<GLenum>(color_bits_per_pixel, trans),
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
			internal_format_from_color_bpp<GLint>(color_bits_per_pixel, trans),
			width,
			height,
			0,
			// todo(Gustav): since we pass null as the data, the type doesn't matter... right?
			GL_RGB,
			GL_UNSIGNED_BYTE,
			nullptr
		);
	}

	// setup fbo
	auto bound = BoundFbo{fbo};
	SET_DEBUG_LABEL_NAMED(fbo->fbo, DebugLabelFor::FrameBuffer, Str() << "FBO " << debug_label);
	constexpr GLint mipmap_level = 0;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, fbo->id, mipmap_level);

	const auto internal_format = determine_fbo_internal_format(include_depth, include_stencil);
	if (internal_format != GL_NONE)
	{
		glGenRenderbuffers(1, &fbo->rbo);
		ASSERT(fbo->rbo != 0);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);
		SET_DEBUG_LABEL_NAMED(fbo->rbo, DebugLabelFor::RenderBuffer, Str() << "TEXTURE RENDBUFF " << debug_label);

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
