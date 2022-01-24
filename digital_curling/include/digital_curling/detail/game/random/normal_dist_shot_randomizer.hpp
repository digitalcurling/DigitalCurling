#ifndef DIGITAL_CURLING_GAME_RANDOM_NORMAL_DIST_SHOT_RANDOMIZER_HPP
#define DIGITAL_CURLING_GAME_RANDOM_NORMAL_DIST_SHOT_RANDOMIZER_HPP

#include <random>
#include <memory>
#include "shot_randomizer.hpp"

namespace digital_curling::game::random {



/// <summary>
/// ショットの初速と角度に正規分布の乱数を加えるIShotRandomizerの実装(シード指定あり)
/// 乱数生成器のシード値は関数Randomizeを呼び出した全てのスレッドでseedの値になります．
/// </summary>
class NormalDistShotRandomizer : public IShotRandomizer {
public:
    /// <summary>
    /// ショットの初速に加わる乱数(正規分布をとる)の標準偏差．
    /// </summary>
    float stddev_speed = 0.001f;

    /// <summary>
    /// ショットの初期角度に加わる乱数(正規分布をとる)の標準偏差．
    /// </summary>
    float stddev_angle = 0.001f;

    /// <summary>
    /// シード値．関数Randomizeを呼び出す全てのスレッドの乱数生成器でこのシード値が使用されます．
    /// </summary>
    std::random_device::result_type seed = 0u;

    NormalDistShotRandomizer();
    NormalDistShotRandomizer(NormalDistShotRandomizer const&);
    NormalDistShotRandomizer & operator = (NormalDistShotRandomizer const&);
    NormalDistShotRandomizer(NormalDistShotRandomizer&&) noexcept;
    NormalDistShotRandomizer & operator = (NormalDistShotRandomizer&&) noexcept;
    virtual ~NormalDistShotRandomizer();

    virtual Vector2 Randomize(Vector2 shot_velocity) override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};



} // namespace digital_curling::game::random

#endif // DIGITAL_CURLING_GAME_RANDOM_NORMAL_DIST_SHOT_RANDOMIZER_HPP
