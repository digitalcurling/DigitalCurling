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

#include "players_identical_player_storage.hpp"

#include "digitalcurling3/detail/players/identical_player_factory.hpp"
#include "players_identical_player.hpp"

namespace digitalcurling3::players {

std::unique_ptr<IPlayer> IdenticalPlayerStorage::CreatePlayer() const
{
    return std::make_unique<IdenticalPlayer>();
}

std::string IdenticalPlayerStorage::GetPlayerId() const
{
    return std::string(kIdenticalPlayerId);
}

// json
void to_json(nlohmann::json & j, IdenticalPlayerStorage const& v)
{
    j["type"] = kIdenticalPlayerId;
}

void from_json(nlohmann::json const& j, IdenticalPlayerStorage & v)
{
    // nothing to do
}

} // namespace digitalcurling3::players
