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

#ifndef DIGITALCURLING3_SRC_PLAYERS_PLAYER_ORIGINAL1_STORAGE_HPP
#define DIGITALCURLING3_SRC_PLAYERS_PLAYER_ORIGINAL1_STORAGE_HPP

#include "digitalcurling3/detail/i_player_storage.hpp"
#include "digitalcurling3/detail/players/player_original1_factory.hpp"

namespace digitalcurling3::players {

class PlayerOriginal1Storage : public IPlayerStorage {
public:
    PlayerOriginal1Storage() = default;
    PlayerOriginal1Storage(PlayerOriginal1Storage const&) = default;
    PlayerOriginal1Storage & operator = (PlayerOriginal1Storage const&) = default;
    virtual ~PlayerOriginal1Storage() = default;

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

    PlayerOriginal1Factory factory;
    std::string engine_data;
    std::string speed_dist_data;
    std::string angle_dist_data;
};


// json
void to_json(nlohmann::json &, PlayerOriginal1Storage const&);
void from_json(nlohmann::json const&, PlayerOriginal1Storage &);

} // namespace digitalcurling3::players


#endif // DIGITALCURLING3_SRC_PLAYERS_PLAYER_ORIGINAL1_STORAGE_HPP
