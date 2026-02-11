// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief SimulateModeFlag を定義

#pragma once

namespace digitalcurling::simulators {

/// @brief シミュレーションの停止条件
enum class SimulateModeFlag {
    /// @brief 全てのストーンが停止するまで
    Full      = 1 << 0,
    /// @brief いずれかのストーンがシート外に出るまで
    OutStone  = 1 << 1,
    /// @brief いずれかのストーンが衝突するまで
    Collision = 1 << 2,
};

} // namespace digitalcurling::simulators
