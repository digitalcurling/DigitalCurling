#ifndef DIGITAL_CURLING_COMMON_HPP
#define DIGITAL_CURLING_COMMON_HPP

#include <cstdint>
#include <cmath>
#include <array>
#include <optional>
#include "nlohmann/json.hpp"

namespace digital_curling {

/// <summary>
/// 円周率
/// </summary>
constexpr float kPi = 3.14159265359f;



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

void to_json(nlohmann::json &, Vector2 const&);
void from_json(nlohmann::json const&, Vector2 &);



/// <summary>
/// ストーンのインデックスに用いる型
/// </summary>
using StoneId = std::uint8_t;



/// <summary>
/// シミュレータ上に配置可能なストーンの最大数
/// </summary>
constexpr StoneId kStoneMax = 16;

} // namespace digital_curling



namespace nlohmann {

// For serializing/deserializing std::optional to/from json
template <typename T>
struct adl_serializer<std::optional<T>> {
    static void to_json(json & j, std::optional<T> const& opt)
    {
        if (opt) {
            j = *opt;
        } else {
            j = nullptr;
        }
    }

    static void from_json(json const& j, std::optional<T> & opt)
    {
        if (j.is_null()) {
            opt = std::nullopt;
        } else {
            opt = j.get<T>();
        }
    }
};

} // namespace nlohmann

#endif // DIGITAL_CURLING_COMMON_HPP
