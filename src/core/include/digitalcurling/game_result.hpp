// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief GameResult を定義

#pragma once

#include <cstdint>
#include "common.hpp"
#include "team.hpp"

namespace digitalcurling {


/// @brief 試合結果
struct GameResult {
    /// @brief 勝敗が定まった理由
    enum class Reason : std::uint8_t {
        /// @brief スコアによる勝敗
        kScore,
        /// @brief コンシードによる勝敗
        kConcede,
        /// @brief 制限時間超過による勝敗
        kTimeLimit,
        /// @brief 引き分け
        kDraw
    };

    /// @brief 勝利チーム
    ///
    /// 引き分けの場合は `Team::kInvalid`
    Team winner = Team::kInvalid;

    /// @brief 勝敗の理由
    Reason reason = Reason::kDraw;
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_JSON_SERIALIZE_ENUM(GameResult::Reason, {
    {GameResult::Reason::kScore, "score"},
    {GameResult::Reason::kConcede, "concede"},
    {GameResult::Reason::kTimeLimit, "time_limit"},
    {GameResult::Reason::kDraw, "draw"},
})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameResult, winner, reason)
/// @endcond

} // namespace digitalcurling
