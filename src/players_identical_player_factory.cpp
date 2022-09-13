#include "digital_curling/detail/players/identical_player_factory.hpp"
#include "players_identical_player.hpp"

namespace digital_curling::players {

std::unique_ptr<IPlayer> IdenticalPlayerFactory::CreatePlayer() const
{
    return std::make_unique<IdenticalPlayer>();
}

std::unique_ptr<IPlayerFactory> IdenticalPlayerFactory::Clone() const
{
    return std::make_unique<IdenticalPlayerFactory>(*this);
}


// json

void to_json(nlohmann::json & j, IdenticalPlayerFactory const& v)
{
    j["type"] = kIdenticalPlayerId;
}

void from_json(nlohmann::json const&, IdenticalPlayerFactory &)
{
    // nothing to do
}

} // namespace digital_curling::players
