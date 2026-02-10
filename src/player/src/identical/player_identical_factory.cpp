// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "digitalcurling3/detail/players/player_identical_factory.hpp"
#include "players_player_identical.hpp"

namespace digitalcurling3::players {

std::unique_ptr<IPlayer> PlayerIdenticalFactory::CreatePlayer() const
{
    return std::make_unique<PlayerIdentical>();
}

std::unique_ptr<IPlayerFactory> PlayerIdenticalFactory::Clone() const
{
    return std::make_unique<PlayerIdenticalFactory>(*this);
}


// json

void to_json(nlohmann::json & j, PlayerIdenticalFactory const& v)
{
    j["type"] = kPlayerIdenticalId;
}

void from_json(nlohmann::json const&, PlayerIdenticalFactory &)
{
    // nothing to do
}

} // namespace digitalcurling3::players
