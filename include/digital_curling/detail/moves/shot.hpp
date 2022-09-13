/// \file
/// \brief Shot を定義します

#ifndef DIGITAL_CURLING_MOVES_SHOT_HPP
#define DIGITAL_CURLING_MOVES_SHOT_HPP

#include <cstdint>
#include "../json/common.hpp"
#include "../vector2.hpp"

namespace digital_curling::moves {



/// \brief 行動：ショット
///
/// \sa Move
struct Shot {
    /// \brief ショットの回転方向
    enum class Rotation : std::uint8_t {
        kCCW, ///< 反時計回り
        kCW   ///< 時計回り
    };

    Vector2 velocity; ///< ショットの初速度

    Rotation rotation = Rotation::kCCW; ///< ショットの初期回転方向
};


// json
NLOHMANN_JSON_SERIALIZE_ENUM(Shot::Rotation, {
    {Shot::Rotation::kCCW, "ccw"},
    {Shot::Rotation::kCW, "cw"},
})

} // namespace digital_curling::moves

#endif // DIGITAL_CURLING_MOVES_SHOT_HPP
