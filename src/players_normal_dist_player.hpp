/// \file
/// \brief NormalDistShotRandomizer を定義します

#ifndef DIGITAL_CURLING_SRC_PLAYERS_NORMAL_DIST_PLAYER_HPP
#define DIGITAL_CURLING_SRC_PLAYERS_NORMAL_DIST_PLAYER_HPP

#include <random>
#include <optional>
#include "digital_curling/detail/i_player.hpp"
#include "digital_curling/detail/players/normal_dist_player_factory.hpp"

namespace digital_curling::players {

class NormalDistPlayerStorage;


class NormalDistPlayer : public IPlayer {
public:

    NormalDistPlayer(NormalDistPlayerFactory const& factory);
    NormalDistPlayer(NormalDistPlayerStorage const& storage);

    virtual ~NormalDistPlayer() = default;

    virtual moves::Shot Play(moves::Shot const& shot) override;
    virtual std::string GetPlayerId() const
    {
        return std::string(kNormalDistPlayerId);
    }
    virtual IPlayerFactory const& GetFactory() const override
    {
        return factory_;
    }
    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const override;
    virtual void Save(IPlayerStorage & storage) const override;
    virtual void Load(IPlayerStorage const& storage) override;

private:
    NormalDistPlayerFactory factory_;
    std::optional<std::mt19937> engine_;
    std::normal_distribution<float> speed_dist_;
    std::normal_distribution<float> angle_dist_;

    void LoadEngine(
        std::string const& engine_data,
        std::string const& speed_dist_data,
        std::string const& angle_dist_data);
};



} // namespace digital_curling::players

#endif // DIGITAL_CURLING_SRC_PLAYERS_NORMAL_DIST_PLAYER_HPP
