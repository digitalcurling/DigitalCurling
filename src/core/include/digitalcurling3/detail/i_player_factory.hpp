// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief IPlayerFactory を定義します

#ifndef DIGITALCURLING3_I_PLAYER_FACTORY_HPP
#define DIGITALCURLING3_I_PLAYER_FACTORY_HPP

#include <memory>
#include <string>
#include "common.hpp"

namespace digitalcurling3 {

class IPlayer;



/// \brief IPlayer を構築するためのクラスです
///
/// IPlayer のインスタンスはこのクラスの派生クラスの CreatePlayer() で生成されます．
class IPlayerFactory {
protected:
    IPlayerFactory() = default;
    IPlayerFactory(IPlayerFactory const&) = default; ///< コピーコンストラクタ．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．
    IPlayerFactory & operator = (IPlayerFactory const&) = default; ///< コピー代入演算子．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．

public:
    virtual ~IPlayerFactory() = default;

    /// \brief プレイヤーを生成する
    ///
    /// \return 生成されたプレイヤー
    virtual std::unique_ptr<IPlayer> CreatePlayer() const = 0;

    /// \brief このインスタンスを複製する
    ///
    /// \returns 複製されたインスタンス
    virtual std::unique_ptr<IPlayerFactory> Clone() const = 0;

    /// \brief 対応するプレイヤーのプレイヤーIDを得る．
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なる．
    ///
    /// \returns プレイヤーID
    virtual std::string GetPlayerId() const = 0;
};

} // namespace digitalcurling3

#endif // DIGITALCURLING3_I_PLAYER_FACTORY_HPP
