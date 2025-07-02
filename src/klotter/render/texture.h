#pragma once

#include <cstdint>

#include "klotter/render/opengl_labels.h"

namespace klotter
{

/** \addtogroup texture Texture
 *  @{
*/

enum class TextureEdge
{
	clamp,
	repeat
};


enum class TextureRenderStyle
{
	pixel,
	mipmap,
	linear
};


enum class Transparency
{
	include,
	exclude
};

/// A single color in a format to load directly into open gl texture(ABGR on little endian).
/// @see \ref color_from_rgba
enum class SingleColor : std::uint32_t {};

constexpr SingleColor color_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return static_cast<SingleColor>((static_cast<uint32_t>(a) << 24) |
		   (static_cast<uint32_t>(b) << 16) |
		   (static_cast<uint32_t>(g) << 8)  |
		   (static_cast<uint32_t>(r)));
}

// todo(Gustav): this doesn't do anything except allow code reuse, remove?
/// Base class for all textures, but only exist due to code reuse and can easily be inlined.
struct BaseTexture
{
	unsigned int id;

	BaseTexture();
	~BaseTexture();

	BaseTexture(const BaseTexture&) = delete;
	void operator=(const BaseTexture&) = delete;

	BaseTexture(BaseTexture&&) noexcept;
	BaseTexture& operator=(BaseTexture&&) noexcept;

	/// clears the loaded texture to a invalid texture
	void unload();
};

struct Texture2d : BaseTexture
{
	Texture2d() = delete;

	/// "internal"
	Texture2d(DEBUG_LABEL_ARG_MANY const void* pixel_data, unsigned int pixel_format, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t);
};

Texture2d load_image_from_color(DEBUG_LABEL_ARG_MANY SingleColor pixel, TextureEdge te, TextureRenderStyle trs, Transparency t);

struct TextureCubemap : BaseTexture
{
	TextureCubemap() = delete;

	TextureCubemap(DEBUG_LABEL_ARG_MANY const std::array<void*, 6>& pixel_data, int width, int height);
};

TextureCubemap load_cubemap_from_color(DEBUG_LABEL_ARG_MANY SingleColor pixel);



/// "render to texture" feature
///	@see \ref FrameBufferBuilder
struct FrameBuffer : BaseTexture
{
	/// @param f The FBO handle
	/// @param w the texture width
	/// @param h the texture height
	FrameBuffer(unsigned int f, int w, int h);
	~FrameBuffer();

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = delete;
	void operator=(const FrameBuffer&) = delete;
	void operator=(FrameBuffer&&) = delete;

	int width;
	int height;

	unsigned int fbo = 0;
	unsigned int rbo = 0;

	bool debug_is_msaa = false;
};

enum class DepthBits
{
	use_none,
	use_16, use_24, use_32
};

/// A builder class for the \ref FrameBuffer
struct FrameBufferBuilder
{
	constexpr explicit FrameBufferBuilder(const glm::ivec2& size)
		: width(size.x)
		, height(size.y)
	{
	}

	int width;
	int height;

	DepthBits include_depth = DepthBits::use_none;
	bool include_stencil = false;

	/// 0 samples == no msaa
	int msaa_samples = 0;

	constexpr FrameBufferBuilder& with_msaa(int samples)
	{
		msaa_samples = samples;
		return *this;
	}

	constexpr FrameBufferBuilder& with_depth(DepthBits bits = DepthBits::use_24)
	{
		include_depth = bits;
		return *this;
	}

	constexpr FrameBufferBuilder& with_stencil()
	{
		include_stencil = true;
		return *this;
	}

	// todo(Gustav): reuse buffers created from an earlier postproc build
	// todo(Gustav): reuse buffers from earlier in the postproc stack, that aren't in use
	[[nodiscard]] std::shared_ptr<FrameBuffer> build(DEBUG_LABEL_ARG_SINGLE) const;
};

/// raii class to render to a FrameBuffer
struct BoundFbo
{
	std::shared_ptr<FrameBuffer> fbo;

	BoundFbo(const BoundFbo&) = delete;
	BoundFbo(BoundFbo&&) = delete;
	void operator=(const BoundFbo&) = delete;
	void operator=(BoundFbo&&) = delete;

	explicit BoundFbo(std::shared_ptr<FrameBuffer> f);
	~BoundFbo();
};

void resolve_multisampled_buffer(const FrameBuffer& src, FrameBuffer* dst);

/**
 * @}
*/

}  //  namespace klotter
