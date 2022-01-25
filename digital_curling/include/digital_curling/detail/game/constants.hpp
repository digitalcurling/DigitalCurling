/// \file
/// \brief ゲームに関する定数を定義します

#ifndef DIGITAL_CURLING_GAME_CONSTANTS_HPP
#define DIGITAL_CURLING_GAME_CONSTANTS_HPP

#include <cstdint>

namespace digital_curling::game {



/// \brief 延長戦を含まないエンド数の最大値
constexpr std::uint8_t kEndMax = 10;



/// \brief 延長戦を含めたエンド数の最大値 ( State::current_end >= kExtraEndMax のときは無効なエンド)
constexpr std::uint8_t kExtraEndMax = 255;



} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_COMMON_HPP
