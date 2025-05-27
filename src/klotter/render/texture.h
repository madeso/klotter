#pragma once

#include "klotter/render/uniform.h"

namespace klotter
{


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

// todo(Gustav): why do the BaseTexture has a width and height when they aren't directly used??
struct BaseTexture
{
	unsigned int id;
	int width;
	int height;

	BaseTexture();	///< creates a invalid texture
	explicit BaseTexture(int w, int h);	 ///< creates a valid texture

	~BaseTexture();

	BaseTexture(const BaseTexture&) = delete;
	void operator=(const BaseTexture&) = delete;

	BaseTexture(BaseTexture&&) noexcept;
	BaseTexture& operator=(BaseTexture&&) noexcept;

	// clears the loaded texture to a invalid texture
	void unload();
};

struct Texture2d : BaseTexture
{
	Texture2d() = default;	///< creates a invalid texture

	/// "internal"
	Texture2d(const void* pixel_data, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t);
};

Texture2d load_image_from_color(u32 pixel, TextureEdge te, TextureRenderStyle trs, Transparency t);

struct TextureCubemap : BaseTexture
{
	TextureCubemap() = default;	 ///< creates a invalid cubemap

	TextureCubemap(const std::array<void*, 6>& pixel_data, int w, int h);
};

TextureCubemap load_cubemap_from_color(u32 pixel);



/// "render to texture" feature
///	@see FrameBufferBuilder
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

	unsigned int fbo = 0;
	unsigned int rbo = 0;

	bool debug_is_msaa = false;
};

struct FrameBufferBuilder
{
	constexpr explicit FrameBufferBuilder(const glm::ivec2& size)
		: width(size.x)
		, height(size.y)
	{
	}

	int width;
	int height;

	/// 0 samples == no msaa
	int msaa_samples = 0;

	constexpr FrameBufferBuilder& with_msaa(int samples)
	{
		msaa_samples = samples;
		return *this;
	}

	// todo(Gustav): reuse buffers created from an earlier FSE build
	// todo(Gustav): reuse buffers from earlier in the FSE stack, that aren't in use
	std::shared_ptr<FrameBuffer> build() const;
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

}  //  namespace klotter
