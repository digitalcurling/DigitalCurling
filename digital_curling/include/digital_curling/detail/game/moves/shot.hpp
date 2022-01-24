/// \file
/// \brief Shot を定義します

#ifndef DIGITAL_CURLING_GAME_MOVES_SHOT_HPP
#define DIGITAL_CURLING_GAME_MOVES_SHOT_HPP

#include "../../vector2.hpp"

namespace digital_curling::game::moves {



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



} // namespace digital_curling::game::moves

#endif // DIGITAL_CURLING_GAME_MOVES_SHOT_HPP
