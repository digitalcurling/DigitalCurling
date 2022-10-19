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

#ifndef DIGITALCURLING3_SRC_PLAYERS_IDENTICAL_PLAYER_STORAGE_HPP
#define DIGITALCURLING3_SRC_PLAYERS_IDENTICAL_PLAYER_STORAGE_HPP

#include "digitalcurling3/detail/i_player_storage.hpp"

namespace digitalcurling3::players {

class IdenticalPlayerStorage : public IPlayerStorage {
public:
    IdenticalPlayerStorage() = default;
    IdenticalPlayerStorage(IdenticalPlayerStorage const&) = default;
    IdenticalPlayerStorage& operator = (IdenticalPlayerStorage const&) = default;
    virtual ~IdenticalPlayerStorage() = default;

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
void to_json(nlohmann::json &, IdenticalPlayerStorage const&);
void from_json(nlohmann::json const&, IdenticalPlayerStorage &);

} // namespace digitalcurling3::players


#endif // DIGITALCURLING3_SRC_PLAYERS_IDENTICAL_PLAYER_STORAGE_HPP
