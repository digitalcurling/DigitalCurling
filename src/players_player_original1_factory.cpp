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

#include "digitalcurling3/detail/players/player_original1_factory.hpp"
#include "players_player_original1.hpp"

namespace digitalcurling3::players {

std::unique_ptr<IPlayer> PlayerOriginal1Factory::CreatePlayer() const
{
    return std::make_unique<PlayerOriginal1>(*this);
}

std::unique_ptr<IPlayerFactory> PlayerOriginal1Factory::Clone() const
{
    return std::make_unique<PlayerOriginal1Factory>(*this);
}


// json
void to_json(nlohmann::json & j, PlayerOriginal1Factory const& v)
{
    j["type"] = kPlayerOriginal1Id;
    j["max_speed"] = v.max_speed;
    j["stddev_speed"] = v.stddev_speed;
    j["stddev_angle"] = v.stddev_angle;
    j["seed"] = v.seed;
}

void from_json(nlohmann::json const& j, PlayerOriginal1Factory & v)
{
    j.at("max_speed").get_to(v.max_speed);
    j.at("stddev_speed").get_to(v.stddev_speed);
    j.at("stddev_angle").get_to(v.stddev_angle);
    j.at("seed").get_to(v.seed);
}

} // namespace digitalcurling3::players
