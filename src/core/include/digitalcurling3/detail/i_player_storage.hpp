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
