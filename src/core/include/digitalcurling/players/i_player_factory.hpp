// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief IPlayerFactory を定義

#pragma once

#include <memory>
#include <string>
#include "digitalcurling/common.hpp"
#include "digitalcurling/players/gender.hpp"
#include "digitalcurling/players/i_player.hpp"

namespace digitalcurling::players {


/// @brief IPlayer を構築するためのファクトリーインターフェース
///
/// `IPlayer` のインスタンスはこのクラスの派生クラスの `CreatePlayer()` で生成されます。
class IPlayerFactory {
protected:
    IPlayerFactory() = default;
    /// @brief コピーコンストラクタ
    IPlayerFactory(IPlayerFactory const&) = default;
    /// @brief コピー代入演算子
    IPlayerFactory & operator = (IPlayerFactory const&) = default;

public:
    virtual ~IPlayerFactory() = default;

    /// @brief プレイヤーを生成する
    /// @return 生成されたプレイヤー
    virtual std::unique_ptr<IPlayer> CreatePlayer() const = 0;

    /// @brief このインスタンスを複製する
    /// @returns 複製されたインスタンス
    virtual std::unique_ptr<IPlayerFactory> Clone() const = 0;

    /// @brief 対応するプレイヤーのプレイヤーIDを得る
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なります。
    ///
    /// @returns プレイヤーID
    virtual std::string GetPlayerId() const = 0;

    /// @brief プレイヤーの性別を得る
    /// @returns プレイヤーの性別
    virtual Gender GetGender() const = 0;
};

} // namespace digitalcurling::players
