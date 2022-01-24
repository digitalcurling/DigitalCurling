#ifndef DIGITAL_CURLING_GAME_SETTING_HPP
#define DIGITAL_CURLING_GAME_SETTING_HPP

#include <cstdint>
#include <memory>
#include <functional>
#include "../simulation/simulator.hpp"
#include "random/shot_randomizer.hpp"

namespace digital_curling::game {

/// <summary>
/// 試合設定．
/// </summary>
struct Setting {
    /// <summary>
    /// ショットの初速度に乱数を加える場合true．
    /// </summary>
    bool randomize_initial_shot_velocity = true;

    /// <summary>
    /// 延長戦を含まないエンド数．kEndMax以下．
    /// </summary>
    std::uint8_t end = 10;

    /// <summary>
    /// アイスシートの横幅．
    /// </summary>
    float sheet_width = 4.75f;

    /// <summary>
    /// ファイブロックルールを適用する場合true．
    /// </summary>
    bool five_rock_rule = true;

    /// <summary>
    /// ショットの最大速度
    /// </summary>
    float max_shot_speed = 3.f;

    /// <summary>
    /// ショットの初速にブレを与えるためのもの
    /// </summary>
    std::shared_ptr<random::IShotRandomizer> shot_randomizer = random::IShotRandomizer::GetDefault();

    /// <summary>
    /// シミュレーションのステップごとに呼び出されるコールバックです．
    /// 引数には<see cref="ApplyMove"/>に渡したシミュレータが渡されます．
    /// このコールバックを使用することで，ストーンが動いている最中の情報を得ることができます．
    /// </summary>
    std::function<void(simulation::ISimulator const &)> on_step;
};

} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_SETTING_HPP
