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
/// \brief IShotRandomizer を定義します

#ifndef DIGITALCURLING3_I_PLAYER_HPP
#define DIGITALCURLING3_I_PLAYER_HPP

#include <string>
#include <memory>
#include "vector2.hpp"
#include "moves/shot.hpp"

namespace digitalcurling3 {

class IPlayerFactory;
class IPlayerStorage;


/// \brief 理想的なショットを与えられた際に，乱数を加えたりしてプレイヤーによるブレを付与する
///
/// IPlayerStorage を用いて内部状態を保存/復元することができます．
/// 状態の保存には IPlayer::CreateStorage() または IPlayer::Save() を，
/// 状態の復元には IPlayerStorage::CreatePlayer() または IPlayer::Load() を使用します．
class IPlayer {
protected:
    IPlayer() = default;
    IPlayer(IPlayer const&) = default; ///< コピーコンストラクタ．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．
    IPlayer & operator = (IPlayer const&) = default; ///< コピー代入演算子．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．

public:
    virtual ~IPlayer() = default;

    /// \brief ショットを行う．
    ///
    /// \param[in] shot 理想的なショット
    /// \returns プレイヤーによってプレイされたショット
    virtual moves::Shot Play(moves::Shot const& shot) = 0;

    /// \brief プレイヤーIDを得る．
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なる．
    ///
    /// \returns プレイヤーID
    virtual std::string GetPlayerId() const = 0;

    /// \brief ファクトリーを得る
    ///
    /// 得られたファクトリーはこの IPlayer インスタンスを生成した IPlayerFactory インスタンスよりも
    /// 詳細な情報を持っている可能性があります(例: インスタンスを生成した IPlayerFactory の
    /// シード値が未指定であっても， GetFactory() の返り値ではシード値が補完されている．など)．
    /// 得られた IPlayerFactory から IPlayer を生成することで，同じ初期条件のプレイヤーを得ることができます．
    ///
    /// \returns ファクトリー
    virtual IPlayerFactory const& GetFactory() const = 0;

    /// \brief ストレージを生成する
    ///
    /// \returns 新たに生成されたストレージ
    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const = 0;

    /// \brief ストレージに状態を保存する
    ///
    /// \param[out] storage ストレージ
    virtual void Save(IPlayerStorage & storage) const = 0;

    /// \brief ストレージから状態を復元する
    ///
    /// \param[in] storage ストレージ
    virtual void Load(IPlayerStorage const& storage) = 0;
};



} // namespace digitalcurling3

#endif // DIGITALCURLING3_I_PLAYER_HPP
