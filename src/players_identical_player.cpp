#include "players_identical_player.hpp"

#include "digital_curling/detail/players/identical_player_factory.hpp"
#include "players_identical_player_storage.hpp"

namespace digital_curling::players {

Vector2 IdenticalPlayer::Play(Vector2 shot_velocity)
{
    return shot_velocity;
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
