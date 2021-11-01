#ifndef DIGITAL_CURLING_GAME_NORMAL_DIST_SHOT_RANDOMIZER_HPP
#define DIGITAL_CURLING_GAME_NORMAL_DIST_SHOT_RANDOMIZER_HPP

#include <random>
#include <memory>
#include "common.hpp"

namespace digital_curling::game {

/// <summary>
/// ショットの初速と角度に正規分布の乱数を加えるIShotRandomizerの実装
/// </summary>
class NormalDistShotRandomizer : public IShotRandomizer {
public:
    static constexpr char kType[] = "normal_dist";

    /// <summary>
    /// ショットの初速に加わる乱数(正規分布をとる)の標準偏差．
    /// </summary>
    float stddev_speed = 0.001f;

    /// <summary>
    /// ショットの初期角度に加わる乱数(正規分布をとる)の標準偏差．
    /// </summary>
    float stddev_angle = 0.001f;

    std::optional<std::random_device::result_type> const seed;

    NormalDistShotRandomizer(std::optional<std::random_device::result_type> const& seed = std::nullopt);
    NormalDistShotRandomizer(NormalDistShotRandomizer&&) noexcept;
    virtual ~NormalDistShotRandomizer();
    virtual Vector2 Randomize(Vector2 shot_velocity) override;
    virtual void ToJson(nlohmann::json & j) const override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace digital_curling::game



// json

namespace nlohmann {

template <>
struct adl_serializer<digital_curling::game::NormalDistShotRandomizer> {
    static digital_curling::game::NormalDistShotRandomizer from_json(const json& j);
    static void to_json(json& j, digital_curling::game::NormalDistShotRandomizer const& t);
};

} // namespace nlohmann

#endif // DIGITAL_CURLING_GAME_NORMAL_DIST_SHOT_RANDOMIZER_HPP
