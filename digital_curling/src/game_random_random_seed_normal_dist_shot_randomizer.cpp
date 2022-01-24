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

} // namespace digital_curling::game::random

