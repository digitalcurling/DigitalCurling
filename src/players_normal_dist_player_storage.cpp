#include "players_normal_dist_player_storage.hpp"
#include "players_normal_dist_player.hpp"

namespace digital_curling::players {

std::unique_ptr<IPlayer> NormalDistPlayerStorage::CreatePlayer() const
{
    return std::make_unique<NormalDistPlayer>(*this);
}

std::string NormalDistPlayerStorage::GetPlayerId() const
{
    return std::string(kNormalDistPlayerId);
}

// json
void to_json(nlohmann::json & j, NormalDistPlayerStorage const& v)
{
    j["type"] = kNormalDistPlayerId;
    j["factory"] = v.factory;
    j["engine_data"] = v.engine_data;
    j["speed_dist_data"] = v.speed_dist_data;
    j["angle_dist_data"] = v.angle_dist_data;
}

void from_json(nlohmann::json const& j, NormalDistPlayerStorage & v)
{
    j.at("factory").get_to(v.factory);
    j.at("engine_data").get_to(v.engine_data);
    j.at("speed_dist_data").get_to(v.speed_dist_data);
    j.at("angle_dist_data").get_to(v.angle_dist_data);
}

} // namespace digital_curling::players
