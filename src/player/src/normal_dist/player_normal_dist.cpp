// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <algorithm>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

#include "player_normal_dist.hpp"

namespace digitalcurling::players {

PlayerNormalDist::PlayerNormalDist(PlayerNormalDistFactory const& factory)
    : factory_(factory)
    , engine_(std::nullopt)
    , ccw_speed_dist_(0.f, factory.ccw.stddev_speed)
    , cw_speed_dist_(0.f, factory.cw.stddev_speed)
    , ccw_angle_dist_(0.f, factory.ccw.stddev_angle)
    , cw_angle_dist_(0.f, factory.cw.stddev_angle)
    , IPlayer()
{
    if (!factory_.seed.has_value()) {
        factory_.seed = std::random_device()();
    }
    engine_.emplace(factory_.seed.value());
}

PlayerNormalDist::PlayerNormalDist(PlayerNormalDistStorage const& storage)
    : factory_(storage.factory)
    , engine_(std::mt19937())
    , ccw_speed_dist_()
    , cw_speed_dist_()
    , ccw_angle_dist_()
    , cw_angle_dist_()
    , IPlayer()
{
    LoadEngine(storage.engine_data, storage.ccw_data, storage.cw_data);
}

moves::Shot PlayerNormalDist::Play(moves::Shot const& shot)
{
    moves::Shot played_shot = shot;

    bool const is_ccw = shot.angular_velocity > 0.f;
    float const max_speed = is_ccw ? factory_.ccw.max_speed : factory_.cw.max_speed;
    std::normal_distribution<float> & speed_dist = is_ccw ? ccw_speed_dist_ : cw_speed_dist_;
    std::normal_distribution<float> & angle_dist = is_ccw ? ccw_angle_dist_ : cw_angle_dist_;

    float const speed = std::min(shot.translational_velocity, max_speed) + speed_dist(engine_.value());
    float const angle = shot.release_angle + angle_dist(engine_.value());
    played_shot = moves::Shot { speed, shot.angular_velocity, angle };

    return played_shot;
}

std::unique_ptr<IPlayerStorage> PlayerNormalDist::CreateStorage() const
{
    auto storage = std::make_unique<PlayerNormalDistStorage>();
    Save(*storage);
    return storage;
}

void PlayerNormalDist::Save(IPlayerStorage & storage) const
{
    auto & s = static_cast<PlayerNormalDistStorage &>(storage);
    s.factory = factory_;

    std::ostringstream s_engine;
    s_engine << *engine_;
    s.engine_data = s_engine.str();

    std::ostringstream s_ccw_speed_dist;
    s_ccw_speed_dist << ccw_speed_dist_;
    s.ccw_data.speed_dist = s_ccw_speed_dist.str();
    std::ostringstream s_ccw_angle_dist;
    s_ccw_angle_dist << ccw_angle_dist_;
    s.ccw_data.angle_dist = s_ccw_angle_dist.str();

    std::ostringstream s_cw_speed_dist;
    s_cw_speed_dist << cw_speed_dist_;
    s.cw_data.speed_dist = s_cw_speed_dist.str();
    std::ostringstream s_cw_angle_dist;
    s_cw_angle_dist << cw_angle_dist_;
    s.cw_data.angle_dist = s_cw_angle_dist.str();
}

void PlayerNormalDist::Load(IPlayerStorage const& storage)
{
    auto const& s = static_cast<PlayerNormalDistStorage const&>(storage);
    factory_ = s.factory;
    LoadEngine(s.engine_data, s.ccw_data, s.cw_data);
}

void PlayerNormalDist::LoadEngine(
    std::string const& engine_data,
    NormalDistData const& ccw_data,
    NormalDistData const& cw_data
) {
    std::istringstream s_engle(engine_data);
    s_engle >> *engine_;

    std::istringstream s_ccw_speed_dist(ccw_data.speed_dist);
    s_ccw_speed_dist >> ccw_speed_dist_;
    std::istringstream s_ccw_angle_dist(ccw_data.angle_dist);
    s_ccw_angle_dist >> ccw_angle_dist_;

    std::istringstream s_cw_speed_dist(cw_data.speed_dist);
    s_cw_speed_dist >> cw_speed_dist_;
    std::istringstream s_cw_angle_dist(cw_data.angle_dist);
    s_cw_angle_dist >> cw_angle_dist_;
}

} // namespace digitalcurling::players
