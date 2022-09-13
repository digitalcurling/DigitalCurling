#ifndef DIGITAL_CURLING_SRC_PLAYERS_IDENTICAL_PLAYER_HPP
#define DIGITAL_CURLING_SRC_PLAYERS_IDENTICAL_PLAYER_HPP

#include "digital_curling/detail/i_player.hpp"

namespace digital_curling::players {



class IdenticalPlayer : public IPlayer {
public:
    IdenticalPlayer() = default;
    virtual ~IdenticalPlayer() = default;
    virtual moves::Shot Play(moves::Shot const& shot) override;
    virtual std::string GetPlayerId() const override;
    virtual IPlayerFactory const& GetFactory() const override;
    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const override;
    virtual void Save(IPlayerStorage & storage) const override;
    virtual void Load(IPlayerStorage const& storage) override;
};



} // namespace digital_curling::players

#endif // DIGITAL_CURLING_SRC_PLAYERS_IDENTICAL_PLAYER_HPP
