#pragma once

namespace klotter
{

constexpr float pi = 3.14159265358979323846f;

/// An angle in both degrees and radians.
struct Angle
{
    [[nodiscard]] constexpr static Angle
    from_degrees(float degrees)
    {
        return Angle(rad_from_deg(degrees));
    }

    [[nodiscard]] constexpr static Angle
    from_radians(float radians)
    {
        return Angle(radians);
    }

    void wrap();


    [[nodiscard]] constexpr float
    as_degrees() const
    {
        return deg_from_rad(radians);
    }

    [[nodiscard]] constexpr float
    as_radians() const
    {
        return radians;
    }

    [[nodiscard]] Angle get_wrapped() const;

    void operator+=(const Angle& rhs);
    void operator-=(const Angle& rhs);
    void operator*=(float rhs);
    void operator/=(float rhs);
    Angle operator-() const;

private:
    float radians;

    constexpr explicit Angle(float r) : radians(r) {}

    [[nodiscard]] static constexpr float
    deg_from_rad(float radians)
    {
        return (180.0f / pi) * radians;
    }

    [[nodiscard]] static constexpr float
    rad_from_deg(float degrees)
    {
        return pi / 180.0f * degrees;
    }
};

constexpr Angle one_turn = Angle::from_radians(pi * 2.0f);
constexpr Angle half_turn = Angle::from_radians(pi);
constexpr Angle quarter_turn = Angle::from_radians(pi / 2.0f);
constexpr Angle no_rotation = Angle::from_radians(0.0f);

float sin(const Angle& ang);
float cos(const Angle& ang);
float tan(const Angle& ang);
Angle asin(float v);
Angle acos(float v);
Angle atan(float v);
Angle atan2(float y, float x);

Angle operator+(const Angle& lhs, const Angle& rhs);
Angle operator-(const Angle& lhs, const Angle& rhs);
Angle operator*(const Angle& lhs, float rhs);
Angle operator/(const Angle& lhs, float rhs);
Angle operator*(float rhs, const Angle& lhs);

bool operator<(const Angle& lhs, const Angle& rhs);
bool operator<=(const Angle& lhs, const Angle& rhs);
bool operator>(const Angle& lhs, const Angle& rhs);
bool operator>=(const Angle& lhs, const Angle& rhs);
}
