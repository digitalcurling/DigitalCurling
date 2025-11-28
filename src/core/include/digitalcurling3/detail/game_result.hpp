// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
