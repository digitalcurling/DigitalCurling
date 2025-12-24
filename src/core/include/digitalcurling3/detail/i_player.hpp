// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief IPlayer を定義

#pragma once

#include <string>
#include <memory>
#include "vector2.hpp"
#include "moves/shot.hpp"

namespace digitalcurling3 {

class IPlayerFactory;
class IPlayerStorage;

/// @brief 選手によるショットのブレを表現するためのインターフェース
///
/// 乱数などによって選手のショットにブレを付与します。
///
/// `IPlayerStorage` を用いて内部状態を保存/復元することが可能です。
/// 状態の保存には `IPlayer::CreateStorage()` または `IPlayer::Save()` を、
/// 状態の復元には `IPlayerStorage::CreatePlayer()` または `IPlayer::Load()` を使用してください。
class IPlayer {
protected:
    IPlayer() = default;
    /// @brief コピーコンストラクタ
    IPlayer(IPlayer const&) = default;
    /// @brief コピー代入演算子
    IPlayer & operator = (IPlayer const&) = default;

public:
    virtual ~IPlayer() = default;

    /// @brief ショットを行う
    /// @param[in] shot 理想的なショット
    /// @returns プレイヤーによってプレイされたショット
    virtual moves::Shot Play(moves::Shot const& shot) = 0;

    /// @brief プレイヤーIDを得る
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なります。
    ///
    /// @returns プレイヤーID
    virtual std::string GetPlayerId() const = 0;

    /// @brief ファクトリーを得る
    ///
    /// 得られたファクトリーはこの `IPlayer` インスタンスを生成した `IPlayerFactory` インスタンスよりも
    /// 詳細な情報を持っている可能性があります。(例: インスタンスを生成した `IPlayerFactory` の
    /// シード値が未指定であっても、 `GetFactory()` の返り値ではシード値が補完されているなど)
    ///
    /// 得られた `IPlayerFactory` から `IPlayer` を生成することで、同じ初期条件のプレイヤーを得ることができます。
    ///
    /// @returns ファクトリー
    virtual IPlayerFactory const& GetFactory() const = 0;

    /// @brief ストレージを生成する
    /// @returns 新たに生成されたストレージ
    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const = 0;

    /// @brief ストレージに状態を保存する
    /// @param[out] storage ストレージ
    virtual void Save(IPlayerStorage & storage) const = 0;

    /// @brief ストレージから状態を復元する
    /// @param[in] storage ストレージ
    virtual void Load(IPlayerStorage const& storage) = 0;
};

} // namespace digitalcurling3
