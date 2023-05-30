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
/// \brief PlayerOriginal1 を定義します

#ifndef DIGITALCURLING3_SRC_PLAYERS_PLAYER_ORIGINAL1_HPP
#define DIGITALCURLING3_SRC_PLAYERS_PLAYER_ORIGINAL1_HPP

#include <random>
#include <optional>
#include "digitalcurling3/detail/i_player.hpp"
#include "digitalcurling3/detail/players/player_original1_factory.hpp"

namespace digitalcurling3::players {

class PlayerOriginal1Storage;


class PlayerOriginal1 : public IPlayer {
public:

    PlayerOriginal1(PlayerOriginal1Factory const& factory);
    PlayerOriginal1(PlayerOriginal1Storage const& storage);

    virtual ~PlayerOriginal1() = default;

    virtual moves::Shot Play(moves::Shot const& shot) override;
    virtual std::string GetPlayerId() const
    {
        return std::string(kPlayerOriginal1Id);
    }
    virtual IPlayerFactory const& GetFactory() const override
    {
        return factory_;
    }
    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const override;
    virtual void Save(IPlayerStorage & storage) const override;
    virtual void Load(IPlayerStorage const& storage) override;

private:
    PlayerOriginal1Factory factory_;
    std::optional<std::mt19937> engine_;
    std::normal_distribution<float> speed_dist_;
    std::normal_distribution<float> angle_dist_;

    void LoadEngine(
        std::string const& engine_data,
        std::string const& speed_dist_data,
        std::string const& angle_dist_data);
};



} // namespace digitalcurling3::players

#endif // DIGITALCURLING3_SRC_PLAYERS_PLAYER_ORIGINAL1_HPP
