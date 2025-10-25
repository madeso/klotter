﻿#pragma once

#include "klotter/render/opengl_labels.h"

#include <cstdint>

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

enum class ColorData
{
	/// Apply transformations to the color data. Texture is a diffuse/albedo map, created with human eyes on a monitor in sRGB space.
	color_data,

	// todo(Gustav): should this be more specific so we could define better default colors if the load fails?
	/// Don't apply transformations to the color data. Texture is a normal/roughness/ao/specular map or similar and created by a software in linear space.
	non_color_data,

	/// Explicitly don't care about the color data but same as \ref non_color_data.
	dont_care
};

/// A single color in a format to load directly into open gl texture(ABGR on little endian).
/// @see \ref color_from_rgba
enum class SingleColor : std::uint32_t {};

/// Constructs a \ref SingleColor value from individual red, green, blue, and alpha components.
/// @param r The red component of the color (0x00 - 0xFF).
/// @param g The green component of the color (0x00 - 0xFF).
/// @param b The blue component of the color (0x00 - 0xFF).
/// @param a The alpha (opacity) component of the color (0x00 - 0xFF).
/// @return A \ref SingleColor value representing the color composed of the specified RGBA components.
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

/// A 3d image texture.
struct Texture2d : BaseTexture
{
	Texture2d() = delete;

	/// "internal"
	Texture2d(DEBUG_LABEL_ARG_MANY const void* pixel_data, unsigned int pixel_format, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd);
};

Texture2d load_image_from_color(DEBUG_LABEL_ARG_MANY SingleColor pixel, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd);

// todo(Gustav): turn into an enum?
/// 0=right(x+), 1=left(x-), 2=top(y+), 3=bottom(y-), 4=front(z+), 5=back(z-)
constexpr std::size_t cubemap_size = 6;

/// A cubemap texture.
/// Useful for skybox rendering or faking reflections from a (static) scene.
struct TextureCubemap : BaseTexture
{
	TextureCubemap() = delete;

	TextureCubemap(DEBUG_LABEL_ARG_MANY const std::array<void*, cubemap_size>& pixel_data, int width, int height, ColorData cd);
};

TextureCubemap load_cubemap_from_color(DEBUG_LABEL_ARG_MANY SingleColor pixel, ColorData cd);



/// "render to texture" feature
///	@see \ref FrameBufferBuilder
struct FrameBuffer : BaseTexture
{
	/// @param f The FBO handle
	/// @param w the texture width
	/// @param h the texture height
	FrameBuffer(unsigned int f, const glm::ivec2& s);
	~FrameBuffer();

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = delete;
	void operator=(const FrameBuffer&) = delete;
	void operator=(FrameBuffer&&) = delete;

	glm::ivec2 size;

	unsigned int fbo = 0;
	unsigned int rbo = 0;

	bool debug_is_msaa = false;
};

/// The number of bits to use for the depth buffer.
enum class DepthBits
{
	use_none,
	use_16, use_24, use_32
};

/// The number of bits/pixel to use for the color buffer.
enum class ColorBitsPerPixel
{
	use_depth,
	use_8, use_16, use_32
};

std::shared_ptr<FrameBuffer> build_simple_framebuffer(DEBUG_LABEL_ARG_MANY const glm::ivec2& size);
std::shared_ptr<FrameBuffer> build_msaa_framebuffer(DEBUG_LABEL_ARG_MANY const glm::ivec2& size, int msaa_samples, ColorBitsPerPixel render_world_color_bits_per_pixel);
std::shared_ptr<FrameBuffer> build_realized_framebuffer(DEBUG_LABEL_ARG_MANY const glm::ivec2& size, ColorBitsPerPixel render_world_color_bits_per_pixel);
std::shared_ptr<FrameBuffer> build_shadow_framebuffer(DEBUG_LABEL_ARG_MANY const glm::ivec2& size);

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
