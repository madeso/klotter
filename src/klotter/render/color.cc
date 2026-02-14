#include "klotter/render/color.h"

#include "imgui.h"

#include <cmath>

namespace klotter
{

/// converts from gamma space to linear space
float linear_from_srgb(float value, float gamma)
{
	// aka srgb_transfer_function_inv
	

	// todo(Gustav): is this srgb or a basic gamma2 transformation? what's the difference? verify all code usage!
	return std::pow(value, gamma);
}


Lin_rgb linear_from_srgb(const Rgb& value, float gamma)
{
	return
	{{
		linear_from_srgb(value.r, gamma),
		linear_from_srgb(value.g, gamma),
		linear_from_srgb(value.b, gamma)
	}};
}


float linear_from_srgb(float a)
{
	return .04045f < a ? powf((a + .055f) / 1.055f, 2.4f) : a / 12.92f;
}

Lin_rgb linear_from_srgb(const Rgb& value)
{
	return {{linear_from_srgb(value.r), linear_from_srgb(value.g), linear_from_srgb(value.b)}};
}



float srgb_from_linear(float a)
{
	return .0031308f >= a ? 12.92f * a : 1.055f * powf(a, .4166666666666667f) - .055f;
}


Rgb srgb_from_linear(const Lin_rgb& value)
{
	return {srgb_from_linear(value.linear[0]), srgb_from_linear(value.linear[1]), srgb_from_linear(value.linear[2])};
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


Lin_rgb gamut_clip_preserve_chroma(const Lin_rgb& rgb)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	const auto L0 = clamp(L, 0, 1);

	const auto t = find_gamut_intersection(a_, b_, L, C, L0);
	const auto L_clipped = L0 * (1 - t) + t * L;
	const auto C_clipped = t * C;

	return linear_from_oklab({L_clipped, C_clipped * a_, C_clipped * b_});
}


Lin_rgb gamut_clip_project_to_0_5(const Lin_rgb& rgb)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	const auto L0 = 0.5f;

	const auto t = find_gamut_intersection(a_, b_, L, C, L0);
	const auto L_clipped = L0 * (1 - t) + t * L;
	const auto C_clipped = t * C;

	return linear_from_oklab({L_clipped, C_clipped * a_, C_clipped * b_});
}


Lin_rgb gamut_clip_project_to_L_cusp(const Lin_rgb& rgb)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
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


Lin_rgb gamut_clip_adaptive_L0_0_5(const Lin_rgb& rgb, float alpha)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	const auto Ld = L - 0.5f;
	const auto e1 = 0.5f + std::abs(Ld) + alpha * C;
	const auto L0 = 0.5f * (1.f + sgn(Ld) * (e1 - std::sqrt(e1 * e1 - 2.f * std::abs(Ld))));

	const auto t = find_gamut_intersection(a_, b_, L, C, L0);
	const auto L_clipped = L0 * (1.f - t) + t * L;
	const auto C_clipped = t * C;

	return linear_from_oklab({L_clipped, C_clipped * a_, C_clipped * b_});
}


Lin_rgb gamut_clip_adaptive_L0_L_cusp(const Lin_rgb& rgb, float alpha)
{
	if (rgb.linear.r < 1 && rgb.linear.g < 1 && rgb.linear.b < 1 &&
		rgb.linear.r > 0 && rgb.linear.g > 0 && rgb.linear.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto L = lab.l;
	const auto eps = 0.00001f;
	const auto C = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	// The cusp is computed here and in find_gamut_intersection, an optimized solution would only compute it once.
	const auto cusp = find_cusp(a_, b_);

	const auto Ld = L - cusp.l;
	const auto k = 2.f * (Ld > 0 ? 1.f - cusp.l : cusp.l);

	const auto e1 = 0.5f * k + std::abs(Ld) + alpha * C / k;
	const auto L0 = cusp.l + 0.5f * (sgn(Ld) * (e1 - std::sqrt(e1 * e1 - 2.f * k * std::abs(Ld))));

	const auto t = find_gamut_intersection(a_, b_, L, C, L0);
	const auto L_clipped = L0 * (1.f - t) + t * L;
	const auto C_clipped = t * C;

	return linear_from_oklab({L_clipped, C_clipped * a_, C_clipped * b_});
}


Lab oklab_from_linear(const Lin_rgb& c)
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

Lin_rgb linear_from_oklab(const Lab& c)
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
	return {.l = c.l, .c = std::sqrt(c.a * c.a + c.b * c.b), .h = klotter::atan2(c.b, c.a)};
}

Lab oklab_from_oklch(const Lch& c)
{
	return
	{
		.l = c.l, .a = c.c *klotter::cos(c.h), .b = c.c *klotter::sin(c.h)
	};
}


// Alternative representation of (L_cusp, C_cusp)
// Encoded so S = C_cusp/L_cusp and T = C_cusp/(1-L_cusp)
// The maximum value for C in the triangle is then found as std::min(S*L, T*(1-L)), for a given L
struct ST
{
	float S;
	float T;
};

// toe function for L_r
float toe(float x)
{
	constexpr float k_1 = 0.206f;
	constexpr float k_2 = 0.03f;
	constexpr float k_3 = (1.f + k_1) / (1.f + k_2);
	return 0.5f * (k_3 * x - k_1 + std::sqrt((k_3 * x - k_1) * (k_3 * x - k_1) + 4 * k_2 * k_3 * x));
}

// inverse toe function for L_r
float toe_inv(float x)
{
	constexpr float k_1 = 0.206f;
	constexpr float k_2 = 0.03f;
	constexpr float k_3 = (1.f + k_1) / (1.f + k_2);
	return (x * x + k_1 * x) / (k_3 * (x + k_2));
}

ST ST_from_cusp(const LC& cusp)
{
	const auto L = cusp.l;
	const auto C = cusp.c;
	return {.S = C / L, .T = C / (1 - L)};
}

Rgb srgb_from_hsv(const HSVal& hsv)
{
	const auto h = hsv.hue;
	const auto s = hsv.saturation;
	const auto v = hsv.value;

	const auto a_ = klotter::cos(h);
	const auto b_ = klotter::sin(h);

	const auto cusp = find_cusp(a_, b_);
	const auto ST_max = ST_from_cusp(cusp);
	const auto S_max = ST_max.S;
	const auto T_max = ST_max.T;
	const auto S_0 = 0.5f;
	const auto k = 1 - S_0 / S_max;

	// first we compute L and V as if the gamut is a perfect triangle:

	// L, C when v==1:
	const auto L_v = 1 - s * S_0 / (S_0 + T_max - T_max * k * s);
	const auto C_v = s * T_max * S_0 / (S_0 + T_max - T_max * k * s);

	auto L = v * L_v;
	auto C = v * C_v;

	// then we compensate for both toe and the curved top part of the triangle:
	const auto L_vt = toe_inv(L_v);
	const auto C_vt = C_v * L_vt / L_v;

	const auto L_new = toe_inv(L);
	C = C * L_new / L;
	L = L_new;

	const auto rgb_scale = linear_from_oklab({L_vt, a_ * C_vt, b_ * C_vt});
	float scale_L = cbrtf(1.f / std::max(std::max(rgb_scale.linear.r, rgb_scale.linear.g), std::max(rgb_scale.linear.b, 0.f)));

	L = L * scale_L;
	C = C * scale_L;

	const auto rgb = linear_from_oklab({L, C * a_, C * b_});
	return srgb_from_linear(rgb);
}

HSVal hsv_from_srgb(const Rgb& rgb)
{
	const auto lab = oklab_from_linear(linear_from_srgb(rgb));

	auto C = std::sqrt(lab.a * lab.a + lab.b * lab.b);
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	auto L = lab.l;
	auto h = klotter::atan2(-lab.b, -lab.a);

	const auto cusp = find_cusp(a_, b_);
	const auto ST_max = ST_from_cusp(cusp);
	const auto S_max = ST_max.S;
	const auto T_max = ST_max.T;
	const auto S_0 = 0.5f;
	const auto k = 1 - S_0 / S_max;

	// first we find L_v, C_v, L_vt and C_vt

	const auto t = T_max / (C + L * T_max);
	const auto L_v = t * L;
	const auto C_v = t * C;

	const auto L_vt = toe_inv(L_v);
	const auto C_vt = C_v * L_vt / L_v;

	// we can then use these to invert the step that compensates for the toe and the curved top part of the triangle:
	const auto rgb_scale = linear_from_oklab({.l = L_vt, .a = a_ * C_vt, .b = b_ * C_vt});
	const auto scale_L = cbrtf(1.f / std::max(std::max(rgb_scale.linear.r, rgb_scale.linear.g), std::max(rgb_scale.linear.b, 0.f)));

	L = L / scale_L;
	C = C / scale_L;

	C = C * toe(L) / L;
	L = toe(L);

	// we can now compute v and s:

	const auto v = L / L_v;
	const auto s = (S_0 + T_max) * C_v / ((T_max * S_0) + T_max * k * C_v);

	return {.hue = h, .saturation = s, .value = v};
}


// Returns a smooth approximation of the location of the cusp
// This polynomial was created by an optimization process
// It has been designed so that S_mid < S_max and T_mid < T_max
ST get_ST_mid(float a_, float b_)
{
	const auto S = 0.11516993f
			+ 1.f
				  / (+7.44778970f + 4.15901240f * b_
					 + a_
						   * (-2.19557347f + 1.75198401f * b_
							  + a_
									* (-2.13704948f - 10.02301043f * b_
									   + a_ * (-4.24894561f + 5.38770819f * b_ + 4.69891013f * a_))));

	const auto T = 0.11239642f
			+ 1.f
				  / (+1.61320320f - 0.68124379f * b_
					 + a_
						   * (+0.40370612f + 0.90148123f * b_
							  + a_
									* (-0.27087943f + 0.61223990f * b_
									   + a_ * (+0.00299215f - 0.45399568f * b_ - 0.14661872f * a_))));

	return {.S = S, .T = T};
}

struct Cs
{
	float C_0;
	float C_mid;
	float C_max;
};

Cs get_Cs(float L, float a_, float b_)
{
	const auto cusp = find_cusp(a_, b_);

	const auto C_max = find_gamut_intersection(a_, b_, L, 1, L);
	const auto ST_max = ST_from_cusp(cusp);

	// Scale factor to compensate for the curved part of gamut shape:
	const auto k = C_max / std::min((L * ST_max.S), (1 - L) * ST_max.T);

	float C_mid;
	{
		const auto ST_mid = get_ST_mid(a_, b_);

		// Use a soft minimum function, instead of a sharp triangle shape to get a smooth value for chroma.
		const auto C_a = L * ST_mid.S;
		const auto C_b = (1.f - L) * ST_mid.T;
		C_mid = 0.9f * k * std::sqrt(std::sqrt(1.f / (1.f / (C_a * C_a * C_a * C_a) + 1.f / (C_b * C_b * C_b * C_b))));
	}

	float C_0;
	{
		// for C_0, the shape is independent of hue, so ST are constant. Values picked to roughly be the average values of ST.
		const auto C_a = L * 0.4f;
		const auto C_b = (1.f - L) * 0.8f;

		// Use a soft minimum function, instead of a sharp triangle shape to get a smooth value for chroma.
		C_0 = std::sqrt(1.f / (1.f / (C_a * C_a) + 1.f / (C_b * C_b)));
	}

	return {.C_0 = C_0, .C_mid = C_mid, .C_max = C_max};
}

Rgb srgb_from_hsl(const HSLig& hsl)
{
	const auto h = hsl.hue;
	const auto s = hsl.saturation;
	const auto l = hsl.lightness;

	if (l == 1.0f)
	{
		return {1.f, 1.f, 1.f};
	}

	else if (l == 0.f)
	{
		return {0.f, 0.f, 0.f};
	}

	const auto a_ = klotter::cos(h);
	const auto b_ = klotter::sin(h);
	const auto L = toe_inv(l);

	const auto cs = get_Cs(L, a_, b_);
	const auto C_0 = cs.C_0;
	const auto C_mid = cs.C_mid;
	const auto C_max = cs.C_max;

	// Interpolate the three values for C so that:
	// At s=0: dC/ds = C_0, C=0
	// At s=0.8: C=C_mid
	// At s=1.0: C=C_max

	const auto mid = 0.8f;
	const auto mid_inv = 1.25f;

	float C, t, k_0, k_1, k_2;

	if (s < mid)
	{
		t = mid_inv * s;

		k_1 = mid * C_0;
		k_2 = (1.f - k_1 / C_mid);

		C = t * k_1 / (1.f - k_2 * t);
	}
	else
	{
		t = (s - mid) / (1 - mid);

		k_0 = C_mid;
		k_1 = (1.f - mid) * C_mid * C_mid * mid_inv * mid_inv / C_0;
		k_2 = (1.f - (k_1) / (C_max - C_mid));

		C = k_0 + t * k_1 / (1.f - k_2 * t);
	}

	const auto rgb = linear_from_oklab({L, C * a_, C * b_});
	return srgb_from_linear(rgb);
}

HSLig hsl_from_srgb(const Rgb& rgb)
{
	const auto lab = oklab_from_linear(linear_from_srgb(rgb));

	const auto C = std::sqrt(lab.a * lab.a + lab.b * lab.b);
	const auto a_ = lab.a / C;
	const auto b_ = lab.b / C;

	const auto L = lab.l;
	const auto h = klotter::atan2(-lab.b, -lab.a);

	const auto cs = get_Cs(L, a_, b_);
	const auto C_0 = cs.C_0;
	const auto C_mid = cs.C_mid;
	const auto C_max = cs.C_max;

	// Inverse of the interpolation in okhsl_to_srgb:

	const auto mid = 0.8f;
	const auto mid_inv = 1.25f;

	float s;
	if (C < C_mid)
	{
		const auto k_1 = mid * C_0;
		const auto k_2 = (1.f - k_1 / C_mid);

		const auto t = C / (k_1 + k_2 * C);
		s = t * mid;
	}
	else
	{
		const auto k_0 = C_mid;
		const auto k_1 = (1.f - mid) * C_mid * C_mid * mid_inv * mid_inv / C_0;
		const auto k_2 = (1.f - (k_1) / (C_max - C_mid));

		const auto t = (C - k_0) / (k_1 + k_2 * (C - k_0));
		s = mid + (1.f - mid) * t;
	}

	const auto l = toe(L);
	return {.hue = h, .saturation = s, .lightness = l};
}

}