#pragma once

#include "klotter/render/texture.h"

namespace klotter
{
/** \addtogroup framebuffer Frame buffer
 *  @{
*/


/// "render to texture" feature
struct FrameBuffer
{
	explicit FrameBuffer(unsigned int fbo);
	~FrameBuffer();

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = delete;
	void operator=(const FrameBuffer&) = delete;
	void operator=(FrameBuffer&&) = delete;

	unsigned int fbo = 0;

	Texture2d texture;
	unsigned int rbo = 0;
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

struct FboSetup
{
	constexpr FboSetup(int w, int h)
		: width(w)
		, height(h)
	{
	}

	int width;
	int height;
};

std::shared_ptr<FrameBuffer> create_frame_buffer(
	const FboSetup& set, TextureEdge te, TextureRenderStyle trs, Transparency trans
);

/**
 * @}
*/

}  //  namespace klotter
