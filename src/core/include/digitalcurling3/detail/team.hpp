// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief Team を定義します

#ifndef DIGITALCURLING3_TEAM_HPP
#define DIGITALCURLING3_TEAM_HPP

#include <cstdint>
#include <cassert>
#include <string>
#include <type_traits>

namespace digitalcurling3 {



/// \brief チームを識別するために用いる列挙体です．
enum class Team : std::int8_t {
    k0 = 0, ///< チーム0．最初のエンドで先攻のチーム．
    k1 = 1, ///< チーム1．最初のエンドで後攻のチーム．
    kInvalid ///< 無効な値
};

/// \brief 相手チームを得る
/// 
/// \param[in] team チーム
/// \return 引数のチームの相手のチーム
inline Team GetOpponentTeam(Team team)
{
    switch (team) {
        case Team::k0: return Team::k1;
        case Team::k1: return Team::k0;
        case Team::kInvalid: return Team::kInvalid;
        default:
            assert(false);
            return Team::kInvalid;
    }
}

/// \brief チームを文字列に変換する
/// 
/// \param[in] team チーム
/// \return k0: \c "team0" , k1: \c "team1" , kInvalid: \c "invalid"
inline std::string ToString(Team team)
{
    switch (team) {
        case Team::k0: return "team0";
        case Team::k1: return "team1";
        case Team::kInvalid: return "invalid";
        default:
            assert(false);
            return std::string();
    }
}

/// \brief enum class を \c std::underlying_type_t に変換する
/// 
/// \param[in] value enum class
/// \return \c static_cast<std::underlying_type_t<T>>(value)
template <class T>
inline std::underlying_type_t<T> ToUType(T value)
{
    return static_cast<std::underlying_type_t<T>>(value);
}


// json
NLOHMANN_JSON_SERIALIZE_ENUM(Team, {
    {Team::k0, "team0"},
    {Team::k1, "team1"},
    {Team::kInvalid, nullptr},
})

} // namespace digitalcurling3

#endif // DIGITALCURLING3_TEAM_HPP
