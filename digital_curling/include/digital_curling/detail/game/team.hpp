/// \file
/// \brief Team を定義します

#ifndef DIGITAL_CURLING_GAME_TEAM_HPP
#define DIGITAL_CURLING_GAME_TEAM_HPP

#include <cstdint>

namespace digital_curling::game {



/// \brief チームを識別するために用いる列挙体です．
enum class Team : std::int8_t {
    k0 = 0, ///< チーム0．最初のエンドで先攻のチーム．
    k1 = 1, ///< チーム1．最初のエンドで後攻のチーム．
    kInvalid ///< 無効な値
};



} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_TEAM_HPP
