#pragma once

#include "klotter/render/blend.h"
#include "klotter/render/compare.h"
#include "klotter/render/constants.h"
#include "klotter/render/cullface.h"
#include "klotter/render/rendermode.h"
#include "klotter/render/stencilaction.h"

#include <optional>

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/

/// Blend mode state consisting of src and dst blend functions
using BlendMode = std::tuple<Blend, Blend>;

/// Stencil function state consisiting of the compare function, the ref and the state.
/// @see \ref StateChanger::stencil_func
using StencilFunc = std::tuple<Compare, i32, u32>;

/// Stencil operation state consisting of the stencil fail, depth fail and the pass action.
using StencilOp = std::tuple<StencilAction, StencilAction, StencilAction>;

/// A "cache" for the current open gl state.
struct OpenglStates
{
	std::optional<bool> cull_face;
	std::optional<CullFace> cull_face_mode;

	std::optional<bool> blending;
	std::optional<BlendMode> blend_mode;

	std::optional<bool> depth_test;
	std::optional<bool> depth_mask;
	std::optional<Compare> depth_func;

	std::optional<bool> stencil_test;
	std::optional<u32> stencil_mask;

	std::optional<RenderMode> render_mode;
	std::optional<StencilFunc> stencil_func;
	std::optional<StencilOp> stencil_op;

	std::optional<int> active_texture;
	std::array<std::optional<unsigned int>, MAX_TEXTURES_SUPPORTED> texture_bound;
};

/**
 * @}
*/

}  //  namespace klotter
