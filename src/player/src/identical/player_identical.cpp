// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "players_player_identical.hpp"

#include "digitalcurling3/detail/players/player_identical_factory.hpp"
#include "players_player_identical_storage.hpp"

namespace digitalcurling3::players {

moves::Shot PlayerIdentical::Play(moves::Shot const& shot)
{
    return shot;
}

IPlayerFactory const& PlayerIdentical::GetFactory() const
{
    static PlayerIdenticalFactory factory;
    return factory;
}

std::string PlayerIdentical::GetPlayerId() const
{
    return std::string(kPlayerIdenticalId);
}

std::unique_ptr<IPlayerStorage> PlayerIdentical::CreateStorage() const
{
    return std::make_unique<PlayerIdenticalStorage>();
}

void PlayerIdentical::Save(IPlayerStorage & storage) const
{
    // nothing to do
}

void PlayerIdentical::Load(IPlayerStorage const& storage)
{
    // nothing to do
}

} // namespace digitalcurling3::players
