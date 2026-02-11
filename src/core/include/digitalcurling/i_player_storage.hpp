// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief IPlayerStorage を定義

#pragma once

#include <memory>
#include <string>
#include "digitalcurling/common.hpp"

namespace digitalcurling {

class IPlayer;

/// @brief IPlayer の状態を保存するストレージインターフェース
///
/// `IPlayer::Load()` や `IPlayerStorage::CreatePlayer()` を用いてストレージから状態を復元した場合、
/// `IPlayer` は `IPlayer::Save()` や `IPlayer::CreateStorage()` を用いてセーブした時点の内部状態を完全に復元できます。
/// すなわち、あるストレージに保存された状態を復元した後の `IPlayer::Play()` の呼出し結果は、そのストレージに保存した時点からの `IPlayer::Play()` の呼出し結果と一致します。
///
/// ストレージは `IPlayer` のそれぞれの実装と対応関係にあるため、 `GetPlayerId()` で得られるIDが一致していない場合は保存/復元はできません。
class IPlayerStorage {
protected:
    IPlayerStorage() = default;
    /// @brief コピーコンストラクタ
    IPlayerStorage(IPlayerStorage const&) = default;
    /// @brief コピー代入演算子
    IPlayerStorage & operator = (IPlayerStorage const&) = default;

public:
    virtual ~IPlayerStorage() = default;

    /// @brief このストレージが持つ状態と同じ状態を持つプレイヤーを生成する
    /// @returns 生成されたプレイヤー
    virtual std::unique_ptr<IPlayer> CreatePlayer() const = 0;

    /// @brief 対応するプレイヤーのプレイヤーIDを得る
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なります。
    ///
    /// @returns プレイヤーID
    virtual std::string GetPlayerId() const = 0;
};

} // namespace digitalcurling
