#ifndef DIGITAL_CURLING_GAME_RANDOM_RANDOM_SEED_NORMAL_DIST_SHOT_RANDOMIZER_HPP
#define DIGITAL_CURLING_GAME_RANDOM_RANDOM_SEED_NORMAL_DIST_SHOT_RANDOMIZER_HPP

#include "shot_randomizer.hpp"

namespace digital_curling::game::random {

/// <summary>
/// ショットの初速と角度に正規分布の乱数を加えるIShotRandomizerの実装(シードがランダムなバージョン)
/// 乱数生成器のシード値は関数Randomizeを呼び出した全てのスレッドでランダムに決定されます．
/// </summary>
class RandomSeedNormalDistShotRandomizer : public IShotRandomizer {
public:
    static constexpr char kType[] = "random_seed_normal_dist";

    /// <summary>
    /// ショットの初速に加わる乱数(正規分布をとる)の標準偏差．
    /// </summary>
    float stddev_speed = 0.001f;

    /// <summary>
    /// ショットの初期角度に加わる乱数(正規分布をとる)の標準偏差．
    /// </summary>
    float stddev_angle = 0.001f;

    RandomSeedNormalDistShotRandomizer();
    RandomSeedNormalDistShotRandomizer(RandomSeedNormalDistShotRandomizer const&);
    RandomSeedNormalDistShotRandomizer & operator = (RandomSeedNormalDistShotRandomizer const&);

    virtual ~RandomSeedNormalDistShotRandomizer();
    virtual Vector2 Randomize(Vector2 shot_velocity) override;
    virtual void ToJson(nlohmann::json & j) const override;
};

} // namespace digital_curling::game::random



// json

namespace digital_curling::game::random {

void to_json(nlohmann::json &, RandomSeedNormalDistShotRandomizer const&);
void from_json(nlohmann::json const&, RandomSeedNormalDistShotRandomizer &);

} // namespace digital_curling::game::random

#endif // DIGITAL_CURLING_GAME_RANDOM_RANDOM_SEED_NORMAL_DIST_SHOT_RANDOMIZER_HPP
