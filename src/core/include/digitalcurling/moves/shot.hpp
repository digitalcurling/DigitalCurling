// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief Shot を定義

#pragma once

#include <cstdint>
#include <string_view>
#include "digitalcurling/common.hpp"
#include "digitalcurling/vector2.hpp"

namespace digitalcurling::moves {


/// @brief 行動：ショット
/// @sa Move
struct Shot {
    /// @brief `Move` の識別用タイプ名
    static constexpr std::string_view kTypeName = "shot";

    /// @brief ショットの並進速度
    float translational_velocity;
    /// @brief ショットの角速度
    float angular_velocity;
    /// @brief ショットの角度 (rad)
    float release_angle;

    /// @brief デフォルトコンストラクタ
    Shot() : translational_velocity(0.f), angular_velocity(0.f), release_angle(0.f) {}

    /// @brief コンストラクタ
    Shot(float translational_velocity, float angular_velocity, float release_angle)
        : translational_velocity(translational_velocity),
          angular_velocity(angular_velocity),
          release_angle(release_angle) {}

    /// @brief ショットの速度ベクトルを得る
    /// @returns ベクトル
    Vector2 ToVector2() const {
        return Vector2(
            translational_velocity * std::cos(release_angle),
            translational_velocity * std::sin(release_angle)
        );
    }
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Shot, translational_velocity, angular_velocity, release_angle);
/// @endcond

} // namespace digitalcurling::moves
