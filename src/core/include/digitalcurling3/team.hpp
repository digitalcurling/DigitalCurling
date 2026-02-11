// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief Team を定義

#pragma once

#include <cstdint>
#include <cassert>
#include <string>
#include <type_traits>

namespace digitalcurling3 {


/// @name Teams
/// @{

/// @brief チーム
enum class Team : std::int8_t {
    /// @brief チーム0 (最初のエンドで先攻のチーム)
    k0 = 0,
    /// @brief チーム1 (最初のエンドで後攻のチーム)
    k1 = 1,
    /// @brief 無効な値
    kInvalid
};

/// @brief 相手チームを得る
/// @param[in] team チーム
/// @return 引数のチームの相手のチーム
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

/// @brief チームを文字列に変換する
/// @param[in] team チーム
/// @return チーム文字列
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

/// @brief enum class を `std::underlying_type_t` に変換する
/// @param[in] value enum class
/// @return `static_cast<std::underlying_type_t<T>>(value)`
template <class T>
inline std::underlying_type_t<T> ToUType(T value)
{
    return static_cast<std::underlying_type_t<T>>(value);
}

/// @}


/// @cond Doxygen_Suppress
// json
NLOHMANN_JSON_SERIALIZE_ENUM(Team, {
    {Team::k0, "team0"},
    {Team::k1, "team1"},
    {Team::kInvalid, nullptr},
})
/// @endcond

} // namespace digitalcurling3
