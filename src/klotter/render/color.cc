#include "klotter/render/color.h"

#include "imgui.h"

#include <cmath>

namespace klotter
{

float linear_from_srgb(float value, float gamma)
{
	// converts from gamma space to linear space

	// todo(Gustav): is this srgb or a basic gamma2 transformation? what's the difference? verify all code usage!
	return std::pow(value, gamma);
}




Lrgb linear_from_srgb(const Color& value, float gamma)
{
	return
	{{
		linear_from_srgb(value.r, gamma),
		linear_from_srgb(value.g, gamma),
		linear_from_srgb(value.b, gamma)
	}};
}


Lab oklab_from_linear(const Lrgb& c)
{
	const auto cr = c.linear.r;
	const auto cg = c.linear.g;
	const auto cb = c.linear.b;

	const auto l = 0.4122214708f * cr + 0.5363325363f * cg + 0.0514459929f * cb;
	const auto m = 0.2119034982f * cr + 0.6806995451f * cg + 0.1073969566f * cb;
	const auto s = 0.0883024619f * cr + 0.2817188376f * cg + 0.6299787005f * cb;

	const auto lp = std::cbrtf(l);
	const auto mp = std::cbrtf(m);
	const auto sp = std::cbrtf(s);

	return {
		.l = 0.2104542553f * lp + 0.7936177850f * mp - 0.0040720468f * sp,
		.a = 1.9779984951f * lp - 2.4285922050f * mp + 0.4505937099f * sp,
		.b = 0.0259040371f * lp + 0.7827717662f * mp - 0.8086757660f * sp,
	};
}

Lrgb linear_from_oklab(const Lab& c)
{
	const auto lp = c.l + 0.3963377774f * c.a + 0.2158037573f * c.b;
	const auto mp = c.l - 0.1055613458f * c.a - 0.0638541728f * c.b;
	const auto sp = c.l - 0.0894841775f * c.a - 1.2914855480f * c.b;

	const auto l = lp * lp * lp;
	const auto m = mp * mp * mp;
	const auto s = sp * sp * sp;

	return {{
		+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
		-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
		-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
	}};
}

// https://en.wikipedia.org/wiki/Oklab_color_space#Conversion_to_and_from_Oklch
Lch oklch_from_oklab(const Lab& c)
{
	return {.l = c.l, .c = std::sqrtf(c.a * c.a + c.b * c.b), .h = klotter::atan2(c.b, c.a)};
}

Lab oklab_from_oklch(const Lch& c)
{
	return
	{
		.l = c.l, .a = c.c *klotter::cos(c.h), .b = c.c *klotter::sin(c.h)
	};
}


}