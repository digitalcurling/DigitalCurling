/// \file
/// \brief RandomSeedNormalDistShotRandomizer を定義します

#ifndef DIGITAL_CURLING_GAME_RANDOM_RANDOM_SEED_NORMAL_DIST_SHOT_RANDOMIZER_HPP
#define DIGITAL_CURLING_GAME_RANDOM_RANDOM_SEED_NORMAL_DIST_SHOT_RANDOMIZER_HPP

#include "shot_randomizer.hpp"

namespace digital_curling::game::random {

/// \brief ショットの初速と角度に正規分布の乱数を加えるIShotRandomizerの実装(シードがランダムなバージョン)
///
/// 乱数生成器のシード値は Randomize() を呼び出した全てのスレッドでランダムに決定されます．
class RandomSeedNormalDistShotRandomizer : public IShotRandomizer {
public:
    /// \brief ショットの初速に加わる正規分布乱数の標準偏差．
    float stddev_speed = 0.001f;

    /// \brief ショットの初期角度に加わる正規分布乱数の標準偏差．
    float stddev_angle = 0.001f;

    /// \brief デフォルトコンストラクタ
    RandomSeedNormalDistShotRandomizer();

    /// \brief コピーコンストラクタ
    RandomSeedNormalDistShotRandomizer(RandomSeedNormalDistShotRandomizer const&);

    /// \brief コピー代入演算子
    RandomSeedNormalDistShotRandomizer & operator = (RandomSeedNormalDistShotRandomizer const&);

    virtual ~RandomSeedNormalDistShotRandomizer();
    virtual Vector2 Randomize(Vector2 shot_velocity) override;
};

} // namespace digital_curling::game::random

#endif // DIGITAL_CURLING_GAME_RANDOM_RANDOM_SEED_NORMAL_DIST_SHOT_RANDOMIZER_HPP
