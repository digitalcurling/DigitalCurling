#include "digital_curling/detail/game/normal_dist_shot_randomizer.hpp"

#include <thread>
#include <mutex>
#include <unordered_map>

namespace digital_curling::game {

class NormalDistShotRandomizer::Impl {
public:
    std::unordered_map<std::thread::id, std::default_random_engine> engines;
    std::mutex mutex;
};

NormalDistShotRandomizer::NormalDistShotRandomizer(std::optional<std::random_device::result_type> const& seed)
    : seed(seed)
    , impl_(std::make_unique<Impl>())
{}

NormalDistShotRandomizer::NormalDistShotRandomizer(NormalDistShotRandomizer&&) noexcept = default;

NormalDistShotRandomizer::~NormalDistShotRandomizer() = default;

Vector2 NormalDistShotRandomizer::Randomize(Vector2 shot_velocity)
{
    auto const thread_id = std::this_thread::get_id();
    std::default_random_engine * engine = nullptr;
    {
        std::lock_guard g(impl_->mutex);
        auto it = impl_->engines.find(thread_id);
        if (it == impl_->engines.end()) {
            // シード値を指定したときはすべてのスレッドでシード値を共有する
            // シード値を指定しなかった場合はすべてのスレッドで別のシード値を用いる
            auto pair = impl_->engines.emplace(thread_id, seed ? *seed : std::random_device()());
            it = pair.first;
        }
        engine = &it->second;
    }

    auto speed_random_dist = std::normal_distribution<float>(0.f, stddev_speed);
    auto angle_random_dist = std::normal_distribution<float>(0.f, stddev_angle);
    float const speed = shot_velocity.Length() + speed_random_dist(*engine);
    float const angle = std::atan2(shot_velocity.y, shot_velocity.x) + angle_random_dist(*engine);
    return speed * Vector2(std::cos(angle), std::sin(angle));
}

void NormalDistShotRandomizer::ToJson(nlohmann::json & j) const
{
    j = *this;
}

} // namespace digital_curling::game



namespace nlohmann {

digital_curling::game::NormalDistShotRandomizer adl_serializer<digital_curling::game::NormalDistShotRandomizer>::from_json(const json& j)
{
    digital_curling::game::NormalDistShotRandomizer v(j.at("seed").get<std::optional<std::random_device::result_type>>());
    j.at("stddev_speed").get_to(v.stddev_speed);
    j.at("stddev_angle").get_to(v.stddev_angle);
    return v;
}

void adl_serializer<digital_curling::game::NormalDistShotRandomizer>::to_json(json& j, digital_curling::game::NormalDistShotRandomizer const& v)
{
    j["type"] = digital_curling::game::NormalDistShotRandomizer::kType;
    j["stddev_speed"] = v.stddev_speed;
    j["stddev_angle"] = v.stddev_angle;
    j["seed"] = v.seed;
}

} // namespace nlohmann
