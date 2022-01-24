#ifndef DIGITAL_CURLING_GAME_MOVES_SHOT_HPP
#define DIGITAL_CURLING_GAME_MOVES_SHOT_HPP

#include "../../vector2.hpp"

namespace digital_curling::game::moves {



/// <summary>
/// 行動：ショット
/// </summary>
struct Shot {
    /// <summary>
    /// ショットの回転方向
    /// </summary>
    enum class Rotation : std::uint8_t {
        /// <summary>反時計回り</summary>
        kCCW,
        /// <summary>時計回り</summary>
        kCW
    };

    /// <summary>
    /// ショットの初速度
    /// </summary>
    Vector2 velocity;

    /// <summary>
    /// ショットの初期回転方向
    /// </summary>
    Rotation rotation = Rotation::kCCW;
};



} // namespace digital_curling::game::moves

#endif // DIGITAL_CURLING_GAME_MOVES_SHOT_HPP
