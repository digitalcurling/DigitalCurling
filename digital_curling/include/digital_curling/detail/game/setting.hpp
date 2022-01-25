/// \file
/// \brief Setting を定義します

#ifndef DIGITAL_CURLING_GAME_SETTING_HPP
#define DIGITAL_CURLING_GAME_SETTING_HPP

#include <cstdint>
#include <memory>
#include <functional>
#include "../simulation/simulator.hpp"
#include "random/shot_randomizer.hpp"

namespace digital_curling::game {

/// \brief 試合設定
struct Setting {

    /// \brief ショットの初速度に乱数を加えるか？
    ///
    /// ショットの初速度に乱数を加える場合は \c true を指定します．
    bool randomize_initial_shot_velocity = true;

    /// \brief 延長戦を含まないエンド数．
    ///
    /// 値は kEndMax 以下である必要があります．
    std::uint8_t end = 10;

    /// \brief アイスシートの横幅．
    float sheet_width = 4.75f;

    /// \brief ファイブロックルールを適用するか？
    ///
    /// ファイブロックルールを適用する場合 \c true を指定します．
    bool five_rock_rule = true;

    /// \brief ショットの最大速度
    float max_shot_speed = 3.f;

    /// \brief ショットの初速に乱数を用いてばらつきを加える方法
    std::shared_ptr<random::IShotRandomizer> shot_randomizer = random::IShotRandomizer::GetDefault();

    /// \brief シミュレーションのステップごとに呼び出されるコールバックです．
    ///
    /// コールバックの引数には ApplyMove() に渡したシミュレータが渡されます．
    /// このコールバックを使用することで， ApplyMove() 中のストーンの軌跡などの情報を得ることができます．
    std::function<void(simulation::ISimulator const &)> on_step;
};

} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_SETTING_HPP
