// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PlayerNormalDist を定義

#pragma once

#include <memory>
#include <optional>
#include <random>
#include <string>
#include "digitalcurling/players/i_player.hpp"

#include "player_normal_dist_factory.hpp"
#include "player_normal_dist_storage.hpp"

namespace digitalcurling::players {

class PlayerNormalDistStorage;

/// @brief ショットの初速に速度上限を適用したのち、初速と角度に正規分布の乱数を加えるプレイヤー
class PlayerNormalDist : public IPlayer {
public:
    /// @brief コンストラクタ
    /// @param factory このプレイヤーのファクトリー
    PlayerNormalDist(PlayerNormalDistFactory const& factory);
    /// @brief コンストラクタ
    /// @param storage このプレイヤーのストレージ
    PlayerNormalDist(PlayerNormalDistStorage const& storage);
    virtual ~PlayerNormalDist() override = default;

    virtual const char* GetId() const noexcept override { return DIGITALCURLING_PLUGIN_NAME; }

    virtual Gender GetGender() const override { return factory_.gender; }
    virtual IPlayerFactory const& GetFactory() const override { return factory_; }

    virtual moves::Shot Play(moves::Shot const& shot) override;

    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const override;
    virtual void Save(IPlayerStorage & storage) const override;
    virtual void Load(IPlayerStorage const& storage) override;

private:
    PlayerNormalDistFactory factory_;
    std::optional<std::mt19937> engine_;
    std::normal_distribution<float> ccw_speed_dist_;
    std::normal_distribution<float> cw_speed_dist_;
    std::normal_distribution<float> ccw_angle_dist_;
    std::normal_distribution<float> cw_angle_dist_;

    void LoadEngine(
        std::string const& engine_data,
        NormalDistData const& ccw_data,
        NormalDistData const& cw_data
    );
};

} // namespace digitalcurling::players
