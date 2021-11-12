#ifndef DIGITAL_CURLING_GAME_CONSTANTS_HPP
#define DIGITAL_CURLING_GAME_CONSTANTS_HPP

#include <cstdint>

namespace digital_curling::game {



/// <summary>
/// 延長戦を含まないエンド数の最大値
/// </summary>
static constexpr std::uint8_t kEndMax = 10;



/// <summary>
/// 延長戦を含めたエンド数の最大値(<see cref="State::current_end"/> >= <see cref="kExtraEndMax"/>のときは無効なエンド)
/// </summary>
static constexpr std::uint8_t kExtraEndMax = 255;



} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_COMMON_HPP
