// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#pragma once

#include "digitalcurling/detail/i_player_storage.hpp"
#include "digitalcurling/detail/players/player_normal_dist_factory.hpp"

namespace digitalcurling::players {

class PlayerNormalDistStorage : public IPlayerStorage {
public:
    PlayerNormalDistStorage() = default;
    PlayerNormalDistStorage(PlayerNormalDistStorage const&) = default;
    PlayerNormalDistStorage & operator = (PlayerNormalDistStorage const&) = default;
    virtual ~PlayerNormalDistStorage() = default;

    /// \brief このストレージが持つ状態と同じ状態を持つプレイヤーを生成する
    ///
    /// \returns 生成されたプレイヤー
    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;

    /// \brief 対応するプレイヤーのプレイヤーIDを得る．
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なる．
    ///
    /// \returns プレイヤーID
    virtual std::string GetPlayerId() const override;

    PlayerNormalDistFactory factory;
    std::string engine_data;
    std::string speed_dist_data;
    std::string angle_dist_data;
};


// json
void to_json(nlohmann::json &, PlayerNormalDistStorage const&);
void from_json(nlohmann::json const&, PlayerNormalDistStorage &);

} // namespace digitalcurling::players
