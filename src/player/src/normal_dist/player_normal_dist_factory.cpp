// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "digitalcurling/detail/players/player_normal_dist_factory.hpp"
#include "players_player_normal_dist.hpp"

namespace digitalcurling::players {

std::unique_ptr<IPlayer> PlayerNormalDistFactory::CreatePlayer() const
{
    return std::make_unique<PlayerNormalDist>(*this);
}

std::unique_ptr<IPlayerFactory> PlayerNormalDistFactory::Clone() const
{
    return std::make_unique<PlayerNormalDistFactory>(*this);
}


// json
void to_json(nlohmann::json & j, PlayerNormalDistFactory const& v)
{
    j["type"] = kPlayerNormalDistId;
    j["max_speed"] = v.max_speed;
    j["stddev_speed"] = v.stddev_speed;
    j["stddev_angle"] = v.stddev_angle;
    j["seed"] = v.seed;
}

void from_json(nlohmann::json const& j, PlayerNormalDistFactory & v)
{
    j.at("max_speed").get_to(v.max_speed);
    j.at("stddev_speed").get_to(v.stddev_speed);
    j.at("stddev_angle").get_to(v.stddev_angle);
    j.at("seed").get_to(v.seed);
}

} // namespace digitalcurling::players
