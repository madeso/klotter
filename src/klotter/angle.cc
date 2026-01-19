#include "klotter/angle.h"
#include "klotter/assert.h"

#include <cmath>

namespace klotter
{
	/// Returns a value that wraps around the range_
	/// When the value go outside the range, the value is wrapped back to either the start if exited at the end or vice versa.
	template<typename T>
	T wrap(T range_lower_bound, T value, T range_upper_bound)
	{
		const T diff = range_upper_bound - range_lower_bound;
		ASSERT(diff > 0);
		T wrapped = value - range_lower_bound;
		while (wrapped < 0)
		{
			wrapped += diff;
		}
		while (wrapped > diff)
		{
			wrapped -= diff;
		}
		return range_lower_bound + wrapped;
	}

    void
    Angle::wrap()
    {
		radians = klotter::wrap(0.0f, radians, pi * 2.0f);
    }


    void
    Angle::operator+=(const Angle& rhs)
    {
        radians += rhs.radians;
    }


    void
    Angle::operator-=(const Angle& rhs)
    {
        radians -= rhs.radians;
    }


    void
    Angle::operator*=(float rhs)
    {
        radians *= rhs;
    }


    void
    Angle::operator/=(float rhs)
    {
        radians /= rhs;
    }


    Angle
    Angle::operator-() const
    {
        return Angle::from_radians(-radians);
    }


    float
    sin(const Angle& ang)
    {
        return std::sin(ang.as_radians());
    }


    float
    cos(const Angle& ang)
    {
        return std::cos(ang.as_radians());
    }


    float
    tan(const Angle& ang)
    {
        return std::tan(ang.as_radians());
    }


    Angle
    asin(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return Angle::from_radians(std::asin(v));
    }


    Angle
    acos(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return Angle::from_radians(std::acos(v));
    }


    Angle
    atan(float v)
    {
        return Angle::from_radians(std::atan(v));
    }


    Angle
    atan2(float y, float x)
    {
        return Angle::from_radians(std::atan2(y, x));
    }


    Angle
    Angle::get_wrapped() const
    {
        Angle temp = *this;
        temp.wrap();
        return temp;
    }


    Angle
    operator+(const Angle& lhs, const Angle& rhs)
    {
        Angle temp(lhs);
        temp += rhs;
        return temp;
    }


    Angle
    operator-(const Angle& lhs, const Angle& rhs)
    {
        Angle temp(lhs);
        temp -= rhs;
        return temp;
    }


    Angle
    operator/(const Angle& lhs, float rhs)
    {
        Angle temp(lhs);
        temp /= rhs;
        return temp;
    }


    Angle operator*(const Angle& lhs, float rhs)
    {
        Angle temp(lhs);
        temp *= rhs;
        return temp;
    }

    Angle operator*(float rhs, const Angle& lhs)
    {
        return lhs * rhs;
    }

    bool
    operator<(const Angle& lhs, const Angle& rhs)
    {
        return lhs.as_radians() < rhs.as_radians();
    }


    bool
    operator<=(const Angle& lhs, const Angle& rhs)
    {
        return lhs.as_radians() <= rhs.as_radians();
    }


    bool
    operator>(const Angle& lhs, const Angle& rhs)
    {
        return lhs.as_radians() > rhs.as_radians();
    }


    bool
    operator>=(const Angle& lhs, const Angle& rhs)
    {
        return lhs.as_radians() >= rhs.as_radians();
    }


    Angle lerp_angle(const Angle& from, float v, const Angle& to)
    {
        // https://gamedev.stackexchange.com/a/72364
        const auto dtheta = to - from;

        const auto new_from = dtheta > half_turn
            ? from + one_turn
            : dtheta < -half_turn
                ? from - one_turn
                : from
            ;
        
        return from + ( to - new_from ) * v;
    }
}
