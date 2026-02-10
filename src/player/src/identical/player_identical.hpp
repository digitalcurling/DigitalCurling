// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#pragma once

#include "digitalcurling3/detail/i_player.hpp"

namespace digitalcurling3::players {



class PlayerIdentical : public IPlayer {
public:
    PlayerIdentical() = default;
    virtual ~PlayerIdentical() = default;
    virtual moves::Shot Play(moves::Shot const& shot) override;
    virtual std::string GetPlayerId() const override;
    virtual IPlayerFactory const& GetFactory() const override;
    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const override;
    virtual void Save(IPlayerStorage & storage) const override;
    virtual void Load(IPlayerStorage const& storage) override;
};



} // namespace digitalcurling3::players
