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
    /// @brief ショットの回転方向
    enum class Rotation : std::uint8_t {
        /// @brief 反時計回り
        kCCW,
        /// @brief 時計回り
        kCW
    };

    /// @brief `Move` の識別用タイプ名
    static constexpr std::string_view kTypeName = "shot";

    /// @brief ショットの初速度
    Vector2 velocity;

    /// @brief ショットの初期回転方向
    Rotation rotation = Rotation::kCCW;
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_JSON_SERIALIZE_ENUM(Shot::Rotation, {
    {Shot::Rotation::kCCW, "ccw"},
    {Shot::Rotation::kCW, "cw"},
})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Shot, velocity, rotation)
/// @endcond

} // namespace digitalcurling::moves
