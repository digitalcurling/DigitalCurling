/// \file
/// \brief NormalDistShotRandomizer を定義します

#ifndef DIGITAL_CURLING_GAME_RANDOM_NORMAL_DIST_SHOT_RANDOMIZER_HPP
#define DIGITAL_CURLING_GAME_RANDOM_NORMAL_DIST_SHOT_RANDOMIZER_HPP

#include <random>
#include <memory>
#include "shot_randomizer.hpp"

namespace digital_curling::game::random {



/// \brief ショットの初速と角度に正規分布の乱数を加えるIShotRandomizerの実装(シード指定あり)
///
/// 乱数生成器のシード値は関数Randomizeを呼び出した全てのスレッドでseedの値になります．
class NormalDistShotRandomizer : public IShotRandomizer {
public:
    /// \brief ショットの初速に加わる正規分布乱数の標準偏差．
    float stddev_speed = 0.001f;

    /// \brief ショットの初期角度に加わる正規分布乱数の標準偏差．
    float stddev_angle = 0.001f;

    /// \brief シード値．
    ///
    /// Randomize() を呼び出す全てのスレッドの乱数生成器でこのシード値が使用されます．
    std::random_device::result_type seed = 0u;

    /// \brief デフォルトコンストラクタ
    NormalDistShotRandomizer();

    /// \brief コピーコンストラクタ
    NormalDistShotRandomizer(NormalDistShotRandomizer const&);

    /// \brief コピー代入演算子
    NormalDistShotRandomizer & operator = (NormalDistShotRandomizer const&);

    /// \brief ムーブコンストラクタ
    NormalDistShotRandomizer(NormalDistShotRandomizer&&) noexcept;

    /// \brief ムーブ代入演算子
    NormalDistShotRandomizer & operator = (NormalDistShotRandomizer&&) noexcept;

    virtual ~NormalDistShotRandomizer();

    virtual Vector2 Randomize(Vector2 shot_velocity) override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};



} // namespace digital_curling::game::random

#endif // DIGITAL_CURLING_GAME_RANDOM_NORMAL_DIST_SHOT_RANDOMIZER_HPP
