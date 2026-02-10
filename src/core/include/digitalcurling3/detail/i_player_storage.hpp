// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief IPlayerStorage を定義します

#ifndef DIGITALCURLING3_I_PLAYER_STORAGE_HPP
#define DIGITALCURLING3_I_PLAYER_STORAGE_HPP

#include <memory>
#include <string>
#include "common.hpp"

namespace digitalcurling3 {

class IPlayer;



/// \brief IPlayer の状態を保存するストレージ
///
/// IPlayer::Load() や IPlayerStorage::CreatePlayer() を用いてストレージから状態を復元した場合，
/// IPlayer は IPlayer::Save() や IPlayer::CreateStorage() を用いてセーブした時点の内部状態を完全に復元します．
/// 言い換えると，あるストレージに保存された状態を復元した後の IPlayer::Play() の呼出し結果は，そのストレージに保存した時点からの IPlayer::Play() の呼出し結果と同じものになります．
///
/// ストレージは IPlayer のそれぞれの実装と対応関係にあるため， GetPlayerId() で得られるIDが一致していない場合は保存/復元はできません．
class IPlayerStorage {
protected:
    IPlayerStorage() = default;
    IPlayerStorage(IPlayerStorage const&) = default; ///< コピーコンストラクタ．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．
    IPlayerStorage & operator = (IPlayerStorage const&) = default; ///< コピー代入演算子．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．

public:
    virtual ~IPlayerStorage() = default;

    /// \brief このストレージが持つ状態と同じ状態を持つプレイヤーを生成する
    ///
    /// \returns 生成されたプレイヤー
    virtual std::unique_ptr<IPlayer> CreatePlayer() const = 0;

    /// \brief 対応するプレイヤーのプレイヤーIDを得る．
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なる．
    ///
    /// \returns プレイヤーID
    virtual std::string GetPlayerId() const = 0;
};

} // namespace digitalcurling3

#endif // DIGITALCURLING3_I_SIMULATOR_STORAGE_HPP
