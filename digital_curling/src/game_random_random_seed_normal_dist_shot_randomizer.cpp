#include "digital_curling/detail/game/random/random_seed_normal_dist_shot_randomizer.hpp"

#include "shot_randomizer_common.hpp"

namespace digital_curling::game::random {

RandomSeedNormalDistShotRandomizer::RandomSeedNormalDistShotRandomizer() = default;

RandomSeedNormalDistShotRandomizer::RandomSeedNormalDistShotRandomizer(RandomSeedNormalDistShotRandomizer const&) = default;

RandomSeedNormalDistShotRandomizer & RandomSeedNormalDistShotRandomizer::operator = (RandomSeedNormalDistShotRandomizer const&) = default;

RandomSeedNormalDistShotRandomizer::~RandomSeedNormalDistShotRandomizer() = default;

Vector2 RandomSeedNormalDistShotRandomizer::Randomize(Vector2 shot_velocity)
{
    thread_local std::default_random_engine engine(std::random_device{}());
    return detail::RandomizeShotVelocityNormalDist(shot_velocity, engine, stddev_speed, stddev_angle);
}

void RandomSeedNormalDistShotRandomizer::ToJson(nlohmann::json & j) const
{
    j = *this;
}

void to_json(nlohmann::json & j, RandomSeedNormalDistShotRandomizer const& v)
{
    j["type"] = RandomSeedNormalDistShotRandomizer::kType;
    j["stddev_speed"] = v.stddev_speed;
    j["stddev_angle"] = v.stddev_angle;
}

void from_json(nlohmann::json const& j, RandomSeedNormalDistShotRandomizer & v)
{
    j.at("stddev_speed").get_to(v.stddev_speed);
    j.at("stddev_angle").get_to(v.stddev_angle);
}

} // namespace digital_curling::game::random

