// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief Vector2 を定義します

#ifndef DIGITALCURLING3_VECTOR2_HPP
#define DIGITALCURLING3_VECTOR2_HPP

#include <cmath>
#include "common.hpp"

namespace digitalcurling3 {



/// \brief 2次元ベクトル
struct Vector2 {
    float x;  ///< x座標
    float y;  ///< y座標

    /// \brief (0, 0)で初期化します
    constexpr Vector2() : x(0.f), y(0.f) {}

    /// \brief 指定された座標で初期化します
    ///
    /// \param[in] x x座標
    /// \param[in] y y座標
    constexpr Vector2(float x, float y) : x(x), y(y) {}

    /// \brief ベクトルの加算を行います
    ///
    /// \param[in] v ベクトル
    /// \returns このベクトル自身
    constexpr Vector2 & operator += (Vector2 v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    /// \brief ベクトルの減算を行います
    ///
    /// \param[in] v ベクトル
    /// \returns このベクトル自身
    constexpr Vector2 & operator -= (Vector2 v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    /// \brief ベクトルにスカラー値を乗算します
    ///
    /// \param[in] f スカラー値
    /// \returns このベクトル自身
    constexpr Vector2 & operator *= (float f)
    {
        x *= f;
        y *= f;
        return *this;
    }

    /// \brief ベクトルをスカラー値で除算します
    ///
    /// \param[in] f スカラー値
    /// \returns このベクトル自身
    constexpr Vector2 & operator /= (float f)
    {
        x /= f;
        y /= f;
        return *this;
    }

    /// \brief ベクトルの長さを得ます
    ///
    /// \returns \code std::hypot(x, y) \endcode
    float Length() const
    {
        return std::hypot(x, y);
    }
};

/// \brief ベクトルを反転します
///
/// \param[in] v ベクトル
/// \returns \a -v
constexpr Vector2 operator - (Vector2 v)
{
    return { -v.x, -v.y };
}

/// \brief ベクトル同士の加算を行います
///
/// \param[in] v1 ベクトル1
/// \param[in] v2 ベクトル2
/// \returns \a v1+v2
constexpr Vector2 operator + (Vector2 v1, Vector2 v2)
{
    return { v1.x + v2.x, v1.y + v2.y };
}

/// \brief ベクトル同士の減算を行います
///
/// \param[in] v1 ベクトル1
/// \param[in] v2 ベクトル2
/// \returns \a v1-v2
constexpr Vector2 operator - (Vector2 v1, Vector2 v2)
{
    return { v1.x - v2.x, v1.y - v2.y };
}

/// \brief ベクトルとスカラー値の乗算を行います
///
/// \param[in] f スカラー値
/// \param[in] v ベクトル
/// \returns \a f*v
constexpr Vector2 operator * (float f, Vector2 v)
{
    return { f * v.x, f * v.y };
}

/// \brief ベクトルとスカラー値の乗算を行います
///
/// \param[in] v ベクトル
/// \param[in] f スカラー値
/// \returns \a v*f
constexpr Vector2 operator * (Vector2 v, float f)
{
    return f * v;
}

/// \brief ベクトルとスカラー値の除算を行います
///
/// \param[in] v ベクトル
/// \param[in] f スカラー値
/// \returns \a v*(1/f)
constexpr Vector2 operator / (Vector2 v, float f)
{
    return { v.x / f, v.y / f };
}


// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Vector2,
    x,
    y
)

} // namespace digitalcurling3

#endif // DIGITALCURLING3_VECTOR2_HPP
