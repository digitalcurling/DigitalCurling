// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PluginPlayer を定義

#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <uuidv7/uuidv7.hpp>
#include "digitalcurling/players/i_player.hpp"

#include "digitalcurling/plugins/detail/plugin_resource.hpp"
#include "digitalcurling/plugins/wrapper_base.hpp"

namespace digitalcurling::players {

/// @brief プラグインを経由したプレイヤー
class PluginPlayer : public plugins::WrapperBase<digitalcurling::players::IPlayer> {
public:
    /// @brief コンストラクタ
    /// @param player_id プレイヤーID
    /// @param instance_id インスタンスID
    /// @param owner_resource 実際のインスタンスを持つリソースへの参照
    PluginPlayer(
        std::string player_id,
        uuidv7::uuidv7 instance_id,
        std::weak_ptr<OwnerResource> owner_resource
    );

    virtual moves::Shot Play(moves::Shot const& shot) override;

    virtual Gender GetGender() const override;
    virtual IPlayerFactory const& GetFactory() const override;

    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const override;
    virtual void Save(IPlayerStorage & storage) const override;
    virtual void Load(IPlayerStorage const& storage) override;

private:
    mutable std::mutex mutex_;
    mutable std::unique_ptr<IPlayerFactory> factory_cache_;

    inline void ClearCaches() {
        std::lock_guard<std::mutex> lock(mutex_);
        factory_cache_.reset();
    }
};

} // namespace digitalcurling::players
