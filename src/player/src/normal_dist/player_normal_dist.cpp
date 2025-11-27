// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "players_player_normal_dist.hpp"

#include <sstream>
#include <algorithm>
#include "players_player_normal_dist_storage.hpp"

namespace digitalcurling3::players {

PlayerNormalDist::PlayerNormalDist(PlayerNormalDistFactory const& factory)
    : factory_(factory)
    , engine_(std::nullopt)
    , speed_dist_(0.f, factory.stddev_speed)
    , angle_dist_(0.f, factory.stddev_angle)
{
    if (!factory_.seed.has_value()) {
        factory_.seed = std::random_device()();
    }
    engine_.emplace(factory_.seed.value());
}

PlayerNormalDist::PlayerNormalDist(PlayerNormalDistStorage const& storage)
    : factory_(storage.factory)
    , engine_(std::mt19937())
    , speed_dist_()
    , angle_dist_()
{
    LoadEngine(storage.engine_data, storage.speed_dist_data, storage.angle_dist_data);
}

moves::Shot PlayerNormalDist::Play(moves::Shot const& shot)
{
    moves::Shot played_shot = shot;

    float const speed = std::min(shot.velocity.Length(), factory_.max_speed) + speed_dist_(engine_.value());
    float const angle = std::atan2(shot.velocity.y, shot.velocity.x) + angle_dist_(engine_.value());
    played_shot.velocity = speed * Vector2(std::cos(angle), std::sin(angle));

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
    auto & s = dynamic_cast<PlayerNormalDistStorage &>(storage);
    s.factory = factory_;

    std::ostringstream s_engine;
    s_engine << *engine_;
    s.engine_data = s_engine.str();

    std::ostringstream s_speed_dist;
    s_speed_dist << speed_dist_;
    s.speed_dist_data = s_speed_dist.str();

    std::ostringstream s_angle_dist;
    s_angle_dist << angle_dist_;
    s.angle_dist_data = s_angle_dist.str();
}

void PlayerNormalDist::Load(IPlayerStorage const& storage)
{
    auto const& s = dynamic_cast<PlayerNormalDistStorage const&>(storage);
    factory_ = s.factory;
    LoadEngine(s.engine_data, s.speed_dist_data, s.angle_dist_data);
}

void PlayerNormalDist::LoadEngine(
    std::string const& engine_data,
    std::string const& speed_dist_data,
    std::string const& angle_dist_data)
{
    std::istringstream s_engle(engine_data);
    s_engle >> *engine_;

    std::istringstream s_speed_dist(speed_dist_data);
    s_speed_dist >> speed_dist_;

    std::istringstream s_angle_dist(angle_dist_data);
    s_angle_dist >> angle_dist_;
}

} // namespace digitalcurling3::players
