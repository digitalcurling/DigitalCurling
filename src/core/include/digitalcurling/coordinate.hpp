// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief 座標関連の定数を定義

#pragma once

#include "digitalcurling/vector2.hpp"

namespace digitalcurling::coordinate {

    /// @brief ホグラインのY座標
    constexpr float kHogLineY = 32.004f;

    /// @brief ティーラインのY座標
    constexpr float kTeeLineY = 38.405f;

    /// @brief バックラインのY座標
    constexpr float kBackLineY = 40.234f;

    /// @brief ハックのY座標
    constexpr float kHackY = 42.063f;

    /// @brief バックボードのY座標
    constexpr float kBackBoardY = 43.892f;

    /// @brief ハウスの半径
    constexpr float kHouseRadius = 1.829f;

    /// @brief ハウスの中心座標
    constexpr static Vector2 kTee = Vector2(0.f, coordinate::kTeeLineY);

} // namespace digitalcurling::coordinate
