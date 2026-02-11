// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief Team, TeamValue を定義

#pragma once

#include <array>
#include <cstdint>
#include <cassert>
#include <string>
#include <type_traits>
#include <nlohmann/json.hpp>
#include "digitalcurling/common.hpp"

namespace digitalcurling {


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


/// @brief チームごとの値
/// @tparam TValue 値の型
template <typename TValue>
struct TeamValue {
public:
    /// @brief チーム0の値
    TValue team0;
    /// @brief チーム1の値
    TValue team1;

    /// @brief コンストラクタ
    TeamValue() : team0(), team1() {}

    /// @brief コンストラクタ
    /// @param[in] value 初期値(team0, team1ともに同じ値)
    TeamValue(TValue value) : team0(value), team1(value) {}

    /// @brief コンストラクタ
    /// @param[in] team0 チーム0の値
    /// @param[in] team1 チーム1の値
    TeamValue(TValue team0, TValue team1) : team0(team0), team1(team1) {}

    /// @brief コピーコンストラクタ
    TeamValue(TeamValue<TValue> const&) = default;
    /// @brief ムーブコンストラクタ
    TeamValue(TeamValue<TValue>&&) = default;

    /// @brief コピー演算子
    TeamValue<TValue>& operator=(TeamValue<TValue> const&) = default;
    /// @brief ムーブ演算子
    TeamValue<TValue>& operator=(TeamValue<TValue>&&) = default;

    /// @brief チームの値を取得する
    /// @param[in] team チーム
    TValue const& operator[](Team team) const {
        switch (team) {
            case Team::k0: return team0;
            case Team::k1: return team1;
            case Team::kInvalid:
            default:
                assert(false);
                return team0;
        }
    }

    /// @brief チームの値を取得する
    ///
    /// 1つ目の値がチーム0、2つ目の値がチーム1に対応します。
    std::array<TValue, 2> ToArray() {
        return { team0, team1 };
    }
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_JSON_SERIALIZE_ENUM(Team, {
    {Team::k0, "team0"},
    {Team::k1, "team1"},
    {Team::kInvalid, nullptr},
})

template <typename T>
void to_json(nlohmann::json & j, digitalcurling::TeamValue<T> const& v) {
    j["team0"] = v.team0;
    j["team1"] = v.team1;
}
template <typename T>
void from_json(nlohmann::json const& j, digitalcurling::TeamValue<T> & v) {
    j.at("team0").get_to(v.team0);
    j.at("team1").get_to(v.team1);
}
/// @endcond

} // namespace digitalcurling
