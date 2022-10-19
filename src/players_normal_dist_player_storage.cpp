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

#include "players_normal_dist_player_storage.hpp"
#include "players_normal_dist_player.hpp"

namespace digitalcurling3::players {

std::unique_ptr<IPlayer> NormalDistPlayerStorage::CreatePlayer() const
{
    return std::make_unique<NormalDistPlayer>(*this);
}

std::string NormalDistPlayerStorage::GetPlayerId() const
{
    return std::string(kNormalDistPlayerId);
}

// json
void to_json(nlohmann::json & j, NormalDistPlayerStorage const& v)
{
    j["type"] = kNormalDistPlayerId;
    j["factory"] = v.factory;
    j["engine_data"] = v.engine_data;
    j["speed_dist_data"] = v.speed_dist_data;
    j["angle_dist_data"] = v.angle_dist_data;
}

void from_json(nlohmann::json const& j, NormalDistPlayerStorage & v)
{
    j.at("factory").get_to(v.factory);
    j.at("engine_data").get_to(v.engine_data);
    j.at("speed_dist_data").get_to(v.speed_dist_data);
    j.at("angle_dist_data").get_to(v.angle_dist_data);
}

} // namespace digitalcurling3::players
