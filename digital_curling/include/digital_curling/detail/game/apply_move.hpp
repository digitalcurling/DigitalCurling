/// \file
/// \brief Move, ApplyMove を定義します

#ifndef DIGITAL_CURLING_GAME_APPLY_MOVE_HPP
#define DIGITAL_CURLING_GAME_APPLY_MOVE_HPP

#include <variant>
#include "../simulation/simulator.hpp"
#include "setting.hpp"
#include "state.hpp"
#include "moves/shot.hpp"
#include "moves/concede.hpp"
#include "moves/time_limit.hpp"

namespace digital_curling::game {



/// \brief ターンごとにチームが採る行動
///
/// \sa moves::Shot, moves::Concede, moves::TimeLimit
using Move = std::variant<moves::Shot, moves::Concede, moves::TimeLimit>;



/// \brief 試合を1ターン進めます
///
/// 複数のスレッドで同時に実行する場合は，すべての引数にスレッド毎に作成したインスタンスを指定してください．
/// \param[in] setting 試合設定
/// \param[in,out] state 試合状態．呼出し後は1ターン後の状態になります．
/// \param[in,out] simulator 物理シミュレータ．
/// \param[in,out] move 行動． moves::Shot の場合は moves::Shot::velocity に乱数が加えられた値が返されます．
void ApplyMove(
    Setting const& setting,
    State & state,
    simulation::ISimulator & simulator,
    Move & move);



} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_APPLY_MOVE_HPP
