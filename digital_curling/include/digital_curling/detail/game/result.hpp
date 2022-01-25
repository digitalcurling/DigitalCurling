/// \file
/// \brief Result を定義します

#ifndef DIGITAL_CURLING_GAME_RESULT_HPP
#define DIGITAL_CURLING_GAME_RESULT_HPP

#include <cstdint>
#include "team.hpp"

namespace digital_curling::game {



/// \brief 試合の結果を格納する．
struct Result {

    /// \brief 勝敗が定まった理由
    enum class Reason : std::uint8_t {
        kScore, ///< スコアによる勝敗
        kConcede, ///< コンシードによる勝敗
        kTimeLimit, ///<制限時間超過による勝敗
        kInvalid ///< 無効な値．引き分けの際はこの値が格納される．
    };

    /// \brief 勝利チーム
    ///
    /// 引き分けの場合は Team::kInvalid が格納される．
    Team win = Team::k0;

    /// \brief 勝敗の理由
    Reason reason = Reason::kScore;
};



} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_RESULT_HPP
