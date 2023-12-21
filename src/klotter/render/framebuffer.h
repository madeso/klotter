#pragma once

#include "klotter/render/texture.h"

namespace klotter
{


struct FrameBuffer
{
	explicit FrameBuffer(unsigned int fbo);
	~FrameBuffer();

	unsigned int fbo;

	Texture texture;
	unsigned int rbo = 0;
};

struct BoundFbo
{
	std::shared_ptr<FrameBuffer> fbo;

	explicit BoundFbo(std::shared_ptr<FrameBuffer> f);
	~BoundFbo();
};

}  //  namespace klotter
