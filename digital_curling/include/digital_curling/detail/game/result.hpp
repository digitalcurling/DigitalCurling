#ifndef DIGITAL_CURLING_GAME_RESULT_HPP
#define DIGITAL_CURLING_GAME_RESULT_HPP

#include <cstdint>
#include "team.hpp"

namespace digital_curling::game {



/// <summary>
/// 試合の結果を格納する．
/// </summary>
struct Result {

    /// <summary>
    /// 勝敗が定まった理由
    /// </summary>
    enum class Reason : std::uint8_t {
        /// <summary>スコアによる勝敗．</summary>
        kScore,
        /// <summary>コンシードによる勝敗．</summary>
        kConcede,
        /// <summary>制限時間超過による勝敗．</summary>
        kTimeLimit,
        /// <summary>引き分けの際はこの値が格納される．</summary>
        kInvalid
    };

    /// <summary>
    /// 勝者．引き分けの場合は<see cref="Team::kInvalid"/>が格納される．
    /// </summary>
    Team win = Team::k0;

    /// <summary>
    /// 勝敗の要因．
    /// </summary>
    Reason reason = Reason::kScore;
};



} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_RESULT_HPP
