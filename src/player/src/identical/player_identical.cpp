// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <memory>
#include "player_identical.hpp"

namespace digitalcurling::players {

PlayerIdentical::PlayerIdentical(PlayerIdenticalFactory const& factory)
    : IPlayer(), gender_(factory.gender) { }

PlayerIdentical::PlayerIdentical(PlayerIdenticalStorage const& storage)
    : IPlayer(), gender_(storage.gender) { }

moves::Shot PlayerIdentical::Play(moves::Shot const& shot)
{
    return shot;
}

IPlayerFactory const& PlayerIdentical::GetFactory() const
{
    static PlayerIdenticalFactory factory;
    factory.gender = gender_;
    return factory;
}

std::unique_ptr<IPlayerStorage> PlayerIdentical::CreateStorage() const
{
    auto storage = std::make_unique<PlayerIdenticalStorage>();
    Save(*storage);
    return storage;
}

void PlayerIdentical::Save(IPlayerStorage & storage) const
{
    auto& s = static_cast<PlayerIdenticalStorage&>(storage);
    s.gender = gender_;
}

void PlayerIdentical::Load(IPlayerStorage const& storage)
{
    auto const& s = static_cast<PlayerIdenticalStorage const&>(storage);
    gender_ = s.gender;
}

} // namespace digitalcurling::players
