// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief Transform を定義

#pragma once

#include "vector2.hpp"
#include "nlohmann/json.hpp"

namespace digitalcurling {


/// @brief 位置と角度を格納する構造体
struct Transform {
    /// @brief 位置
    Vector2 position;
    /// @brief 角度
    float angle;

    /// @brief 位置(0, 0)、角度0で初期化する
    constexpr Transform() : position(), angle(0.f) {}

    /// @brief 指定された値で初期化する
    /// @param[in] position 位置
    /// @param[in] angle 角度
    constexpr Transform(Vector2 position, float angle) : position(position), angle(angle) {}
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform, position, angle)
/// @endcond

} // namespace digitalcurling
