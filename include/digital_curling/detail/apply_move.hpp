/// \file
/// \brief ApplyMove(), ApplyMoveResult を定義します

#ifndef DIGITAL_CURLING_APPLY_MOVE_HPP
#define DIGITAL_CURLING_APPLY_MOVE_HPP

#include <functional>
#include <chrono>
#include "json/common.hpp"
#include "game_setting.hpp"
#include "i_simulator.hpp"
#include "i_player.hpp"
#include "game_state.hpp"
#include "move.hpp"


namespace digital_curling {


/// \brief ApplyMove() の返り値
struct ApplyMoveResult {
    bool free_guard_zone_foul; ///< フリーガードゾーンルールに違反した場合 \c true
};


/// \brief 試合を1ターン進めます
///
/// 複数のスレッドで同時に実行する場合は，すべての引数にスレッド毎に作成したインスタンスを指定してください．
///
/// 引数の \c simulator や \c shot_randomizer のメンバ関数を呼び出した際に例外が送出された場合の動作は保証しません．
///
/// \param[in] setting この試合設定に基づいて試合を進めます．
/// \param[in,out] simulator このインスタンスを使用して，ストーンのシミュレーションを行います．
///      呼出し後はすべてのストーンが停止し，プレイエリア外のストーンが除外された時点の状態になります．
/// \param[in,out] player このインスタンスの IPlayer::Play() を呼び出してショットの初速に乱数を加えたり，ショットの初速上限を適用したりします．
/// \param[in,out] state 現在の試合状態を指定します．呼出し後は1ターン後の状態になります．
/// \param[in,out] move この行動が適用されます．呼出し後は実際に行われた行動が格納されます
///     ( moves::Shot の場合に \c player を用いて乱数が加えられます)
/// \param[in] thinking_time_used このターンに使用した思考時間(ミリ秒)
/// \param[out] result 返り値．このターンで起こったことに関する情報が格納されます．
/// \param[in] on_step シミュレーションのステップごとに呼び出されるコールバックです．
///     コールバックの引数には ApplyMove() に渡したシミュレータが渡されます．
///     このコールバックを使用することで， ApplyMove() 中のストーンの軌跡などの情報を得ることができます．
/// \exception std::out_of_range GameState::scores の \c std::vector のサイズが足りない(< GameSetting::max_end)とき
/// \exception std::invalid_argument 上記以外で引数が不正なとき
void ApplyMove(
    GameSetting const& setting,
    ISimulator & simulator,
    IPlayer & player,
    GameState & state,
    Move & move,
    std::chrono::milliseconds const& thinking_time_used,
    ApplyMoveResult * result = nullptr,
    std::function<void(ISimulator const &)> on_step = nullptr);




/// \cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ApplyMoveResult,
    free_guard_zone_foul
)
/// \endcond

} // namespace digital_curling


#endif // DIGITAL_CURLING_APPLY_MOVE_HPP
