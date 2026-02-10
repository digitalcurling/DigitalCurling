// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief Result を定義します

#ifndef DIGITALCURLING3_GAME_RESULT_HPP
#define DIGITALCURLING3_GAME_RESULT_HPP

#include <cstdint>
#include "common.hpp"
#include "team.hpp"

namespace digitalcurling3 {



/// \brief 試合の結果を格納する．
struct GameResult {

    /// \brief 勝敗が定まった理由
    enum class Reason : std::uint8_t {
        kScore, ///< スコアによる勝敗
        kConcede, ///< コンシードによる勝敗
        kTimeLimit, ///<制限時間超過による勝敗
        kDraw ///< 引き分けの際に使用される値
    };

    /// \brief 勝利チーム
    ///
    /// 引き分けの場合は Team::kInvalid が格納される．
    Team winner = Team::kInvalid;

    /// \brief 勝敗の理由
    Reason reason = Reason::kDraw;
};


/// \cond Doxygen_Suppress
// json
NLOHMANN_JSON_SERIALIZE_ENUM(GameResult::Reason, {
    {GameResult::Reason::kScore, "score"},
    {GameResult::Reason::kConcede, "concede"},
    {GameResult::Reason::kTimeLimit, "time_limit"},
    {GameResult::Reason::kDraw, "draw"},
})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    GameResult,
    winner,
    reason
)
/// \endcond

} // namespace digitalcurling3

#endif // DIGITALCURLING3_GAME_RESULT_HPP
