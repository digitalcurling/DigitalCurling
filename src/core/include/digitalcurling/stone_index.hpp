// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief StoneIndex を定義

#pragma once

#include <cstdint>
#include <utility>
#include <nlohmann/json.hpp>
#include "digitalcurling/team.hpp"

namespace digitalcurling {


/// @brief ストーンのインデックス
struct StoneIndex {
    /// @brief チーム
    Team team;
    /// @brief ストーンの番号
    std::uint8_t stone;

    /// @brief コンストラクタ
    StoneIndex() : team(Team::kInvalid), stone(0) {}

    /// @brief コンストラクタ
    /// @param[in] team チーム
    /// @param[in] stone ストーンの番号
    StoneIndex(Team team, std::uint8_t stone) : team(team), stone(stone) {
        if (team != Team::k0 && team != Team::k1) this->team = Team::kInvalid;
    }

    /// @brief チームとストーンの番号のペアに変換する
    operator std::pair<Team, std::uint8_t>() const {
        return { team, stone };
    }
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StoneIndex, team, stone)
/// @endcond

} // namespace digitalcurling
