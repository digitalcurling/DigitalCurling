// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#ifndef DIGITALCURLING3_SRC_PLAYERS_PLAYER_IDENTICAL_STORAGE_HPP
#define DIGITALCURLING3_SRC_PLAYERS_PLAYER_IDENTICAL_STORAGE_HPP

#include "digitalcurling3/detail/i_player_storage.hpp"

namespace digitalcurling3::players {

class PlayerIdenticalStorage : public IPlayerStorage {
public:
    PlayerIdenticalStorage() = default;
    PlayerIdenticalStorage(PlayerIdenticalStorage const&) = default;
    PlayerIdenticalStorage& operator = (PlayerIdenticalStorage const&) = default;
    virtual ~PlayerIdenticalStorage() = default;

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
};


// json
void to_json(nlohmann::json &, PlayerIdenticalStorage const&);
void from_json(nlohmann::json const&, PlayerIdenticalStorage &);

} // namespace digitalcurling3::players


#endif // DIGITALCURLING3_SRC_PLAYERS_PLAYER_IDENTICAL_STORAGE_HPP
