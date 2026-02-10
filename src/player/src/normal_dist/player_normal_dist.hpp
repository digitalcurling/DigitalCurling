// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief PlayerNormalDist を定義します

#pragma once

#include <random>
#include <optional>
#include "digitalcurling3/detail/i_player.hpp"
#include "digitalcurling3/detail/players/player_normal_dist_factory.hpp"

namespace digitalcurling3::players {

class PlayerNormalDistStorage;


class PlayerNormalDist : public IPlayer {
public:

    PlayerNormalDist(PlayerNormalDistFactory const& factory);
    PlayerNormalDist(PlayerNormalDistStorage const& storage);

    virtual ~PlayerNormalDist() = default;

    virtual moves::Shot Play(moves::Shot const& shot) override;
    virtual std::string GetPlayerId() const
    {
        return std::string(kPlayerNormalDistId);
    }
    virtual IPlayerFactory const& GetFactory() const override
    {
        return factory_;
    }
    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const override;
    virtual void Save(IPlayerStorage & storage) const override;
    virtual void Load(IPlayerStorage const& storage) override;

private:
    PlayerNormalDistFactory factory_;
    std::optional<std::mt19937> engine_;
    std::normal_distribution<float> speed_dist_;
    std::normal_distribution<float> angle_dist_;

    void LoadEngine(
        std::string const& engine_data,
        std::string const& speed_dist_data,
        std::string const& angle_dist_data);
};



} // namespace digitalcurling3::players
