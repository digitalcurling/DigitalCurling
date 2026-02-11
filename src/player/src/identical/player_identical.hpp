// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PlayerIdentical を定義

#pragma once

#include <memory>
#include "digitalcurling/players/i_player.hpp"

#include "player_identical_factory.hpp"
#include "player_identical_storage.hpp"

namespace digitalcurling::players {


/// @brief 乱数を加えないプレイヤー
class PlayerIdentical : public IPlayer {
public:
    /// @brief コンストラクタ
    /// @param factory このプレイヤーのファクトリー
    PlayerIdentical(PlayerIdenticalFactory const& factory);
    /// @brief コンストラクタ
    /// @param storage このプレイヤーのストレージ
    PlayerIdentical(PlayerIdenticalStorage const& storage);
    virtual ~PlayerIdentical() override = default;

    virtual const char* GetId() const noexcept override { return DIGITALCURLING_PLUGIN_NAME; }

    virtual Gender GetGender() const override { return gender_; }
    virtual IPlayerFactory const& GetFactory() const override;

    virtual moves::Shot Play(moves::Shot const& shot) override;

    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const override;
    virtual void Save(IPlayerStorage & storage) const override;
    virtual void Load(IPlayerStorage const& storage) override;

private:
    Gender gender_;
};

} // namespace digitalcurling::players
