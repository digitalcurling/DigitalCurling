#include "digital_curling/detail/game/random/normal_dist_shot_randomizer.hpp"

#include <thread>
#include <mutex>
#include <unordered_map>
#include "shot_randomizer_common.hpp"

namespace digital_curling::game::random {

class NormalDistShotRandomizer::Impl {
public:
    Impl() = default;
    Impl(const Impl& rhs) : data(rhs.data) {}

    struct Data {
        Data(std::random_device::result_type seed)
            : engine(seed)
            , seed(seed) {}
        Data(Data const&) = default;
        Data & operator = (Data const&) = default;
        std::default_random_engine engine;
        std::random_device::result_type seed;
    };
    std::unordered_map<std::thread::id, Data> data;
    std::mutex mutex;
};

NormalDistShotRandomizer::NormalDistShotRandomizer()
    : impl_(std::make_unique<Impl>())
{}

NormalDistShotRandomizer::NormalDistShotRandomizer(NormalDistShotRandomizer const& rhs)
    : stddev_speed(rhs.stddev_speed)
    , stddev_angle(rhs.stddev_angle)
    , seed(rhs.seed)
    , impl_(std::make_unique<Impl>(*rhs.impl_))
{}

NormalDistShotRandomizer & NormalDistShotRandomizer::operator = (NormalDistShotRandomizer const& rhs)
{
    if (this == &rhs) return *this;
    this->stddev_speed = rhs.stddev_speed;
    this->stddev_angle = rhs.stddev_angle;
    this->seed = rhs.seed;
    this->impl_ = std::make_unique<Impl>(*rhs.impl_);
    return *this;
}

NormalDistShotRandomizer::NormalDistShotRandomizer(NormalDistShotRandomizer&&) noexcept = default;

NormalDistShotRandomizer & NormalDistShotRandomizer::operator = (NormalDistShotRandomizer&&) noexcept = default;

NormalDistShotRandomizer::~NormalDistShotRandomizer() = default;

Vector2 NormalDistShotRandomizer::Randomize(Vector2 shot_velocity)
{
    auto const thread_id = std::this_thread::get_id();
    std::default_random_engine * engine = nullptr;
    {
        std::lock_guard g(impl_->mutex);
        auto it = impl_->data.find(thread_id);
        if (it == impl_->data.end()) {
            // すべてのスレッドでシード値を共有する
            auto pair = impl_->data.emplace(thread_id, seed);
            it = pair.first;
        } else {
            if (it->second.seed != seed) {
                it->second = Impl::Data(seed);  // シードが変更された場合，再生成する．
            }
        }
        engine = &it->second.engine;
    }

    return detail::RandomizeShotVelocityNormalDist(shot_velocity, *engine, stddev_speed, stddev_angle);
}

} // namespace digital_curling::game
