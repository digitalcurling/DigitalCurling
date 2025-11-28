// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
