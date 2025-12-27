// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief IPlayerFactory を定義

#pragma once

#include <memory>
#include <string>
#include "digitalcurling/common.hpp"
#include "digitalcurling/players/i_player.hpp"
#include "digitalcurling/plugins/i_plugin_object.hpp"

namespace digitalcurling::players {


/// @brief IPlayer を構築するためのファクトリーインターフェース
///
/// `IPlayer` のインスタンスはこのクラスの派生クラスの `CreatePlayer()` で生成されます。
class IPlayerFactory : public plugins::IPluginObjectCreator, public plugins::FactoryHandle {
public:
    IPlayerFactory() = default;
    /// @brief コピーコンストラクタ
    IPlayerFactory(IPlayerFactory const&) = default;
    /// @brief コピー代入演算子
    IPlayerFactory & operator = (IPlayerFactory const&) = default;
    virtual ~IPlayerFactory() override = default;

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
    virtual std::string GetPlayerId() const { return std::string(GetId()); }

    /// @brief プレイヤーの性別を得る
    /// @returns プレイヤーの性別
    virtual Gender GetGender() const = 0;

    virtual plugins::TargetHandle* Create() const override {
        return CreatePlayer().release();
    }
};

} // namespace digitalcurling::players
