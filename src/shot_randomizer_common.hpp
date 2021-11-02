#ifndef DIGITAL_CURLING_SRC_SHOT_RANDOMIZER_COMMON_HPP
#define DIGITAL_CURLING_SRC_SHOT_RANDOMIZER_COMMON_HPP

#include <random>
#include "digital_curling/detail/common.hpp"

namespace digital_curling::game::detail {

inline Vector2 RandomizeShotVelocityNormalDist(Vector2 shot_velocity, std::default_random_engine& engine, float stddev_speed, float stddev_angle)
{
    auto speed_random_dist = std::normal_distribution<float>(0.f, stddev_speed);
    auto angle_random_dist = std::normal_distribution<float>(0.f, stddev_angle);
    float const speed = shot_velocity.Length() + speed_random_dist(engine);
    float const angle = std::atan2(shot_velocity.y, shot_velocity.x) + angle_random_dist(engine);
    return speed * Vector2(std::cos(angle), std::sin(angle));
}

} // namespace digital_curling::game::detail

#endif // DIGITAL_CURLING_SRC_SHOT_RANDOMIZER_COMMON_HPP
