#include "players_identical_player.hpp"

#include "digital_curling/detail/players/identical_player_factory.hpp"
#include "players_identical_player_storage.hpp"

namespace digital_curling::players {

moves::Shot IdenticalPlayer::Play(moves::Shot const& shot)
{
    return shot;
}

IPlayerFactory const& IdenticalPlayer::GetFactory() const
{
    static IdenticalPlayerFactory factory;
    return factory;
}

std::string IdenticalPlayer::GetPlayerId() const
{
    return std::string(kIdenticalPlayerId);
}

std::unique_ptr<IPlayerStorage> IdenticalPlayer::CreateStorage() const
{
    return std::make_unique<IdenticalPlayerStorage>();
}

void IdenticalPlayer::Save(IPlayerStorage & storage) const
{
    // nothing to do
}

void IdenticalPlayer::Load(IPlayerStorage const& storage)
{
    // nothing to do
}

} // namespace digital_curling::players
