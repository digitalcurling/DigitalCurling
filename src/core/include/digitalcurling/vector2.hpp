// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief Vector2 を定義

#pragma once

#include <cmath>
#include <nlohmann/json.hpp>

namespace digitalcurling {


/// @brief 2次元ベクトル
struct Vector2 {
    /// @brief x座標
    float x;
    /// @brief y座標
    float y;

    /// @brief (0, 0)で初期化する
    constexpr Vector2() : x(0.f), y(0.f) {}

    /// @brief 指定された座標で初期化する
    /// @param[in] x x座標
    /// @param[in] y y座標
    constexpr Vector2(float x, float y) : x(x), y(y) {}

    /// @brief ベクトルの長さの二乗を得る
    /// @returns ベクトルの長さの二乗 (`x*x + y*y`)
    constexpr float SquaredLength() const
    {
        return x * x + y * y;
    }

    /// @brief ベクトルの長さを得る
    /// @returns ベクトルの長さ (`std::hypot(x, y)`)
    float Length() const
    {
        return std::hypot(x, y);
    }


    /// @cond Doxygen_Suppress
    // equation operators
    friend constexpr bool operator == (Vector2 v1, Vector2 v2) {
        return v1.x == v2.x && v1.y == v2.y;
    }
    friend constexpr bool operator != (Vector2 v1, Vector2 v2) {
        return !(v1 == v2);
    }

    // arithmetic operators
    constexpr Vector2 & operator += (Vector2 v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    constexpr Vector2 & operator -= (Vector2 v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    constexpr Vector2 & operator *= (float f) {
        x *= f;
        y *= f;
        return *this;
    }
    constexpr Vector2 & operator /= (float f) {
        x /= f;
        y /= f;
        return *this;
    }

    constexpr Vector2 operator-(Vector2 v) {
        return { -v.x, -v.y };
    }
    friend constexpr Vector2 operator+(Vector2 v1, Vector2 v2) {
        return { v1.x + v2.x, v1.y + v2.y };
    }
    friend constexpr Vector2 operator-(Vector2 v1, Vector2 v2) {
        return { v1.x - v2.x, v1.y - v2.y };
    }

    friend constexpr Vector2 operator*(float f, Vector2 v) {
        return { f * v.x, f * v.y };
    }
    friend constexpr Vector2 operator*(Vector2 v, float f) {
        return f * v;
    }
    friend constexpr Vector2 operator/(Vector2 v, float f) {
        return { v.x / f, v.y / f };
    }
    /// @endcond
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector2, x, y)
/// @endcond

} // namespace digitalcurling
