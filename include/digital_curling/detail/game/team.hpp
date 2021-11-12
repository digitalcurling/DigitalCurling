#ifndef DIGITAL_CURLING_GAME_TEAM_HPP
#define DIGITAL_CURLING_GAME_TEAM_HPP

#include <cstdint>
#include "../json.hpp"

namespace digital_curling::game {



/// <summary>
/// チームを識別するために用いる
/// </summary>
enum class Team : std::int8_t {
    /// <summary>
    /// チーム0．最初のエンドで先攻のチーム．
    /// </summary>
    k0 = 0,

    /// <summary>
    /// チーム1．最初のエンドで後攻のチーム．
    /// </summary>
    k1 = 1,

    /// <summary>
    /// 無効な値
    /// </summary>
    kInvalid
};



} // namespace digital_curling::game



// json

namespace digital_curling::game {

NLOHMANN_JSON_SERIALIZE_ENUM(Team, {
    {Team::k0, 0},
    {Team::k1, 1},
    {Team::kInvalid, nullptr},
})

} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_TEAM_HPP
