// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief Shot を定義します

#ifndef DIGITALCURLING3_MOVES_SHOT_HPP
#define DIGITALCURLING3_MOVES_SHOT_HPP

#include <cstdint>
#include <string_view>
#include "../common.hpp"
#include "../vector2.hpp"

namespace digitalcurling3::moves {


/// \brief 行動：ショット
///
/// \sa Move
struct Shot {
    /// \brief ショットの回転方向
    enum class Rotation : std::uint8_t {
        kCCW, ///< 反時計回り
        kCW   ///< 時計回り
    };

    /// \brief `Move` の識別用タイプ名
    static constexpr std::string_view kTypeName = "shot";

    Vector2 velocity; ///< ショットの初速度

    Rotation rotation = Rotation::kCCW; ///< ショットの初期回転方向
};


// json
NLOHMANN_JSON_SERIALIZE_ENUM(Shot::Rotation, {
    {Shot::Rotation::kCCW, "ccw"},
    {Shot::Rotation::kCW, "cw"},
})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Shot, velocity, rotation)

} // namespace digitalcurling3::moves

#endif // DIGITALCURLING3_MOVES_SHOT_HPP
