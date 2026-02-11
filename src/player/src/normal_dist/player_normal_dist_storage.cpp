// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "players_player_normal_dist_storage.hpp"
#include "players_player_normal_dist.hpp"

namespace digitalcurling::players {

std::unique_ptr<IPlayer> PlayerNormalDistStorage::CreatePlayer() const
{
    return std::make_unique<PlayerNormalDist>(*this);
}

std::string PlayerNormalDistStorage::GetPlayerId() const
{
    return std::string(kPlayerNormalDistId);
}

// json
void to_json(nlohmann::json & j, PlayerNormalDistStorage const& v)
{
    j["type"] = kPlayerNormalDistId;
    j["factory"] = v.factory;
    j["engine_data"] = v.engine_data;
    j["speed_dist_data"] = v.speed_dist_data;
    j["angle_dist_data"] = v.angle_dist_data;
}

void from_json(nlohmann::json const& j, PlayerNormalDistStorage & v)
{
    j.at("factory").get_to(v.factory);
    j.at("engine_data").get_to(v.engine_data);
    j.at("speed_dist_data").get_to(v.speed_dist_data);
    j.at("angle_dist_data").get_to(v.angle_dist_data);
}

} // namespace digitalcurling::players
