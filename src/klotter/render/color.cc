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


float srgb_from_linear(float lin)
{
	// https://youtu.be/_zQ_uBAHA4A?t=1333
	return std::max(1.055f * std::pow(lin, 0.416666667f) - 0.055f, 0.0f);
}


Color srgb_from_linear(const Lrgb& value)
{
	return {srgb_from_linear(value.linear[0]), srgb_from_linear(value.linear[1]), srgb_from_linear(value.linear[2])};
}


Color srgb_from_hsl(const Hsl& hsl)
{
	// https://www.rapidtables.com/convert/color/hsl-to-rgb.html

	const auto h = hsl.h.as_degrees();
	const auto s = hsl.s;
	const auto l = hsl.l;

	const auto c = (1.0f - std::fabs(2.0f * l - 1.0f)) * s;
	const auto x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));

	const auto m = l - c / 2.0f;

	float r1;
	float g1;
	float b1;

	if (h < 60.0f) {
		r1 = c;
		g1 = x;
		b1 = 0;
	} else if (h < 120.0f) {
		r1 = x;
		g1 = c;
		b1 = 0;
	} else if (h < 180.0f) {
		r1 = 0;
		g1 = c;
		b1 = x;
	} else if (h < 240.0f) {
		r1 = 0;
		g1 = x;
		b1 = c;
	} else if (h < 300.0f) {
		r1 = x;
		g1 = 0;
		b1 = c;
	} else {
		r1 = c;
		g1 = 0;
		b1 = x;
	}

	
	const auto r = r1 + m;
	const auto g = g1 + m;
	const auto b = b1 + m;

	return {r, g, b};
}


// Finds the maximum saturation possible for a given hue that fits in sRGB
// Saturation here is defined as S = C/L
// a and b must be normalized so a^2 + b^2 == 1
float compute_max_saturation(float a, float b)
{
	// Max saturation will be when one of r, g or b goes below zero.

	// Select different coefficients depending on which component goes below zero first
	float k0, k1, k2, k3, k4, wl, wm, ws;

	if (-1.88170328f * a - 0.80936493f * b > 1)
	{
		// Red component
		k0 = +1.19086277f;
		k1 = +1.76576728f;
		k2 = +0.59662641f;
		k3 = +0.75515197f;
		k4 = +0.56771245f;
		wl = +4.0767416621f;
		wm = -3.3077115913f;
		ws = +0.2309699292f;
	}
	else if (1.81444104f * a - 1.19445276f * b > 1)
	{
		// Green component
		k0 = +0.73956515f;
		k1 = -0.45954404f;
		k2 = +0.08285427f;
		k3 = +0.12541070f;
		k4 = +0.14503204f;
		wl = -1.2684380046f;
		wm = +2.6097574011f;
		ws = -0.3413193965f;
	}
	else
	{
		// Blue component
		k0 = +1.35733652f;
		k1 = -0.00915799f;
		k2 = -1.15130210f;
		k3 = -0.50559606f;
		k4 = +0.00692167f;
		wl = -0.0041960863f;
		wm = -0.7034186147f;
		ws = +1.7076147010f;
	}

	// Approximate max saturation using a polynomial:
	float S = k0 + k1 * a + k2 * b + k3 * a * a + k4 * a * b;

	// Do one step Halley's method to get closer
	// this gives an error less than 10e6, except for some blue hues where the dS/dh is close to infinite
	// this should be sufficient for most applications, otherwise do two/three steps

	float k_l = +0.3963377774f * a + 0.2158037573f * b;
	float k_m = -0.1055613458f * a - 0.0638541728f * b;
	float k_s = -0.0894841775f * a - 1.2914855480f * b;

	{
		float l_ = 1.f + S * k_l;
		float m_ = 1.f + S * k_m;
		float s_ = 1.f + S * k_s;

		float l = l_ * l_ * l_;
		float m = m_ * m_ * m_;
		float s = s_ * s_ * s_;

		float l_dS = 3.f * k_l * l_ * l_;
		float m_dS = 3.f * k_m * m_ * m_;
		float s_dS = 3.f * k_s * s_ * s_;

		float l_dS2 = 6.f * k_l * k_l * l_;
		float m_dS2 = 6.f * k_m * k_m * m_;
		float s_dS2 = 6.f * k_s * k_s * s_;

		float f = wl * l + wm * m + ws * s;
		float f1 = wl * l_dS + wm * m_dS + ws * s_dS;
		float f2 = wl * l_dS2 + wm * m_dS2 + ws * s_dS2;

		S = S - f * f1 / (f1 * f1 - 0.5f * f * f2);
	}

	return S;
}


struct LC
{
	float l;
	float c;
};


/// finds L_cusp and C_cusp for a given hue
/// a and b must be normalized so a^2 + b^2 == 1
LC find_cusp(float a, float b)
{
	// First, find the maximum saturation (saturation S = C/L)
	float S_cusp = compute_max_saturation(a, b);

	// Convert to linear sRGB to find the first point where at least one of r,g or b >= 1:
	const auto rgb_at_max = linear_from_oklab({1, S_cusp * a, S_cusp * b});
	float L_cusp = cbrtf(1.f / std::max(std::max(rgb_at_max.linear.r, rgb_at_max.linear.g), rgb_at_max.linear.b));
	float C_cusp = L_cusp * S_cusp;

	return {L_cusp, C_cusp};
}


/// Finds intersection of the line defined by
/// `L = L0 * (1 - t) + t * L1;`
/// `C = t * C1;`
/// a and b must be normalized so a^2 + b^2 == 1
float find_gamut_intersection(float a, float b, float L1, float C1, float L0)
{
	// Find the cusp of the gamut triangle
	LC cusp = find_cusp(a, b);

	// Find the intersection for upper and lower half separately
	float t;
	if (((L1 - L0) * cusp.c - (cusp.l - L0) * C1) <= 0.f)
	{
		// Lower half

		t = cusp.c * L0 / (C1 * cusp.l + cusp.c * (L0 - L1));
	}
	else
	{
		// Upper half

		// First intersect with triangle
		t = cusp.c * (L0 - 1.f) / (C1 * (cusp.l - 1.f) + cusp.c * (L0 - L1));

		// Then one-step Halley's method
		{
			float dL = L1 - L0;
			float dC = C1;

			float k_l = +0.3963377774f * a + 0.2158037573f * b;
			float k_m = -0.1055613458f * a - 0.0638541728f * b;
			float k_s = -0.0894841775f * a - 1.2914855480f * b;

			float l_dt = dL + dC * k_l;
			float m_dt = dL + dC * k_m;
			float s_dt = dL + dC * k_s;


			// If higher accuracy is required, 2 or 3 iterations of the following block can be used:
			{
				float L = L0 * (1.f - t) + t * L1;
				float C = t * C1;

				float l_ = L + C * k_l;
				float m_ = L + C * k_m;
				float s_ = L + C * k_s;

				float l = l_ * l_ * l_;
				float m = m_ * m_ * m_;
				float s = s_ * s_ * s_;

				float ldt = 3 * l_dt * l_ * l_;
				float mdt = 3 * m_dt * m_ * m_;
				float sdt = 3 * s_dt * s_ * s_;

				float ldt2 = 6 * l_dt * l_dt * l_;
				float mdt2 = 6 * m_dt * m_dt * m_;
				float sdt2 = 6 * s_dt * s_dt * s_;

				float r = 4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s - 1;
				float r1 = 4.0767416621f * ldt - 3.3077115913f * mdt + 0.2309699292f * sdt;
				float r2 = 4.0767416621f * ldt2 - 3.3077115913f * mdt2 + 0.2309699292f * sdt2;

				float u_r = r1 / (r1 * r1 - 0.5f * r * r2);
				float t_r = -r * u_r;

				float g = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s - 1;
				float g1 = -1.2684380046f * ldt + 2.6097574011f * mdt - 0.3413193965f * sdt;
				float g2 = -1.2684380046f * ldt2 + 2.6097574011f * mdt2 - 0.3413193965f * sdt2;

				float u_g = g1 / (g1 * g1 - 0.5f * g * g2);
				float t_g = -g * u_g;

				float b0 = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s - 1;
				float b1 = -0.0041960863f * ldt - 0.7034186147f * mdt + 1.7076147010f * sdt;
				float b2 = -0.0041960863f * ldt2 - 0.7034186147f * mdt2 + 1.7076147010f * sdt2;

				float u_b = b1 / (b1 * b1 - 0.5f * b0 * b2);
				float t_b = -b0 * u_b;

				t_r = u_r >= 0.f ? t_r : FLT_MAX;
				t_g = u_g >= 0.f ? t_g : FLT_MAX;
				t_b = u_b >= 0.f ? t_b : FLT_MAX;

				t += std::min(t_r, std::min(t_g, t_b));
			}
		}
	}

	return t;
}


float clamp(float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;

	return x;
}


float sgn(float x)
{
	// todo(Gustav): make this clearer
	return static_cast<float>(0.f < x) - static_cast<float>(x < 0.f);
}


Lrgb gamut_clip_preserve_chroma(const Lrgb& rgb)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, sqrtf(lab.a * lab.a + lab.b * lab.b));
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	const auto L0 = clamp(L, 0, 1);

	const auto t = find_gamut_intersection(a_, b_, L, C, L0);
	const auto L_clipped = L0 * (1 - t) + t * L;
	const auto C_clipped = t * C;

	return linear_from_oklab({L_clipped, C_clipped * a_, C_clipped * b_});
}


Lrgb gamut_clip_project_to_0_5(const Lrgb& rgb)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, sqrtf(lab.a * lab.a + lab.b * lab.b));
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	const auto L0 = 0.5f;

	const auto t = find_gamut_intersection(a_, b_, L, C, L0);
	const auto L_clipped = L0 * (1 - t) + t * L;
	const auto C_clipped = t * C;

	return linear_from_oklab({L_clipped, C_clipped * a_, C_clipped * b_});
}


Lrgb gamut_clip_project_to_L_cusp(const Lrgb& rgb)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, sqrtf(lab.a * lab.a + lab.b * lab.b));
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	// The cusp is computed here and in find_gamut_intersection, an optimized solution would only compute it once.
	const auto cusp = find_cusp(a_, b_);

	const auto L0 = cusp.l;

	const auto t = find_gamut_intersection(a_, b_, L, C, L0);

	const auto L_clipped = L0 * (1 - t) + t * L;
	const auto C_clipped = t * C;

	return linear_from_oklab({L_clipped, C_clipped * a_, C_clipped * b_});
}


Lrgb gamut_clip_adaptive_L0_0_5(const Lrgb& rgb, float alpha)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, sqrtf(lab.a * lab.a + lab.b * lab.b));
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	const auto Ld = L - 0.5f;
	const auto e1 = 0.5f + fabs(Ld) + alpha * C;
	const auto L0 = 0.5f * (1.f + sgn(Ld) * (e1 - sqrtf(e1 * e1 - 2.f * fabs(Ld))));

	const auto t = find_gamut_intersection(a_, b_, L, C, L0);
	const auto L_clipped = L0 * (1.f - t) + t * L;
	const auto C_clipped = t * C;

	return linear_from_oklab({L_clipped, C_clipped * a_, C_clipped * b_});
}


Lrgb gamut_clip_adaptive_L0_L_cusp(const Lrgb& rgb, float alpha)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, sqrtf(lab.a * lab.a + lab.b * lab.b));
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	// The cusp is computed here and in find_gamut_intersection, an optimized solution would only compute it once.
	const auto cusp = find_cusp(a_, b_);

	const auto Ld = L - cusp.l;
	const auto k = 2.f * (Ld > 0 ? 1.f - cusp.l : cusp.l);

	const auto e1 = 0.5f * k + fabs(Ld) + alpha * C / k;
	const auto L0 = cusp.l + 0.5f * (sgn(Ld) * (e1 - sqrtf(e1 * e1 - 2.f * k * fabs(Ld))));

	const auto t = find_gamut_intersection(a_, b_, L, C, L0);
	const auto L_clipped = L0 * (1.f - t) + t * L;
	const auto C_clipped = t * C;

	return linear_from_oklab({L_clipped, C_clipped * a_, C_clipped * b_});
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