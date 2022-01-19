#ifndef DIGITAL_CURLING_VECTOR2_HPP
#define DIGITAL_CURLING_VECTOR2_HPP

#include <cmath>
#include "json.hpp"

namespace digital_curling {



/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 {
    float x;
    float y;

    constexpr Vector2() : x(0.f), y(0.f) {}
    constexpr Vector2(float x, float y) : x(x), y(y) {}

    constexpr Vector2 & operator += (Vector2 v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    constexpr Vector2 & operator -= (Vector2 v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    constexpr Vector2 & operator *= (float f)
    {
        x *= f;
        y *= f;
        return *this;
    }

    constexpr Vector2 & operator /= (float f)
    {
        x /= f;
        y /= f;
        return *this;
    }

    float Length() const
    {
        return std::hypot(x, y);
    }
};

// 単項
constexpr Vector2 operator - (Vector2 v)
{
    return { -v.x, -v.y };
}

constexpr Vector2 operator + (Vector2 v1, Vector2 v2)
{
    return { v1.x + v2.x, v1.y + v2.y };
}

constexpr Vector2 operator - (Vector2 v1, Vector2 v2)
{
    return { v1.x - v2.x, v1.y - v2.y };
}

constexpr Vector2 operator * (float f, Vector2 v)
{
    return { f * v.x, f * v.y };
}

constexpr Vector2 operator * (Vector2 v, float f)
{
    return f * v;
}

constexpr Vector2 operator / (Vector2 v, float f)
{
    return { v.x / f, v.y / f };
}



} // namespace digital_curling



// json

namespace digital_curling {

void to_json(nlohmann::json &, Vector2 const&);
void from_json(nlohmann::json const&, Vector2 &);

} // namespace digital_curling


#endif // DIGITAL_CURLING_VECTOR2_HPP
