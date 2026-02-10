// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "players_player_identical_storage.hpp"

#include "digitalcurling3/detail/players/player_identical_factory.hpp"
#include "players_player_identical.hpp"

namespace digitalcurling3::players {

std::unique_ptr<IPlayer> PlayerIdenticalStorage::CreatePlayer() const
{
    return std::make_unique<PlayerIdentical>();
}

std::string PlayerIdenticalStorage::GetPlayerId() const
{
    return std::string(kPlayerIdenticalId);
}

// json
void to_json(nlohmann::json & j, PlayerIdenticalStorage const& v)
{
    j["type"] = kPlayerIdenticalId;
}

void from_json(nlohmann::json const& j, PlayerIdenticalStorage & v)
{
    // nothing to do
}

} // namespace digitalcurling3::players
