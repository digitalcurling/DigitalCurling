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



/// <summary>
/// ターンごとにチームが採る行動
/// </summary>
using Move = std::variant<moves::Shot, moves::Concede, moves::TimeLimit>;



/// <summary>
/// 行動を適用し，試合を進める．
/// </summary>
/// <remarks>
/// 複数のスレッドで同時に実行する場合は，すべての引数にスレッド毎に作成したインスタンスを指定してください．
/// </remarks>
/// <param name="setting">試合設定(入力)</param>
/// <param name="state">試合状態(入力および出力)</param>
/// <param name="simulator">ストーンの物理シミュレータ(入力および出力)</param>
/// <param name="move">行動(入力および出力)．ショットの場合は乱数が加えられた値を返す．</param>
void ApplyMove(
    Setting const& setting,
    State & state,
    simulation::ISimulator & simulator,
    Move & move);



} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_APPLY_MOVE_HPP
