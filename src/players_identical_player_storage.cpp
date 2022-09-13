#include "players_identical_player_storage.hpp"

#include "digital_curling/detail/players/identical_player_factory.hpp"
#include "players_identical_player.hpp"

namespace digital_curling::players {

std::unique_ptr<IPlayer> IdenticalPlayerStorage::CreatePlayer() const
{
    return std::make_unique<IdenticalPlayer>();
}

std::string IdenticalPlayerStorage::GetPlayerId() const
{
    return std::string(kIdenticalPlayerId);
}

// json
void to_json(nlohmann::json & j, IdenticalPlayerStorage const& v)
{
    j["type"] = kIdenticalPlayerId;
}

void from_json(nlohmann::json const& j, IdenticalPlayerStorage & v)
{
    // nothing to do
}

} // namespace digital_curling::players
