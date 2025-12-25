// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief Stone を定義

#pragma once

#include "digitalcurling/common.hpp"
#include "digitalcurling/coordinate.hpp"
#include "digitalcurling/vector2.hpp"

namespace digitalcurling {


/// @brief ストーンの位置と角度
struct Stone {

    /// @brief ストーンの半径(m)
    static constexpr float kRadius = 0.145f;

    /// @brief ストーンの位置
    Vector2 position;
    /// @brief ストーンの角度
    float angle;

    /// @brief 位置(0, 0)、角度0で初期化する
    constexpr Stone() : position(), angle(0.f) {}

    /// @brief 指定された値で初期化する
    /// @param[in] position 位置
    /// @param[in] angle 角度
    constexpr Stone(Vector2 position, float angle) : position(position), angle(angle) {}

    /// @brief ストーンの中心からハウスの中心までの距離を取得する
    /// @return ハウス-ストーン間の距離
    float GetDistanceFromTee() const {
        return (position - coordinate::kTee).Length();
    }

    /// @brief ストーンがハウスの中にあるかを判定する
    /// @return ストーンがハウスの中にあれば `true`
    bool IsInHouse() const {
        return GetDistanceFromTee() < coordinate::kHouseRadius + kRadius;
    }
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Stone, position, angle)
/// @endcond

} // namespace digitalcurling
