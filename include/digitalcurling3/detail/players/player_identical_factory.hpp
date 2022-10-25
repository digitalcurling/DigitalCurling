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
/// \brief PlayerIdenticalFactory を定義します

#ifndef DIGITALCURLING3_PLAYERS_PLAYER_IDENTICAL_FACTORY_HPP
#define DIGITALCURLING3_PLAYERS_PLAYER_IDENTICAL_FACTORY_HPP

#include <memory>
#include "../json/common.hpp"
#include "../i_player_factory.hpp"

namespace digitalcurling3::players {

/// \brief プレイヤーIdenticalのID
constexpr std::string_view kPlayerIdenticalId = "identical";


/// \brief 乱数を加えないプレイヤー
class PlayerIdenticalFactory : public IPlayerFactory {
public:
    PlayerIdenticalFactory() = default;  ///< デフォルトコンストラクタ
    virtual ~PlayerIdenticalFactory() = default; 
    PlayerIdenticalFactory(PlayerIdenticalFactory const&) = default;  ///< コピーコンストラクタ
    PlayerIdenticalFactory & operator = (PlayerIdenticalFactory const&) = default;  ///< コピー代入演算子

    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;
    virtual std::unique_ptr<IPlayerFactory> Clone() const override;
    virtual std::string GetPlayerId() const override
    {
        return std::string(kPlayerIdenticalId);
    }
};


/// \cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, PlayerIdenticalFactory const&);
void from_json(nlohmann::json const&, PlayerIdenticalFactory &);
/// \endcond

} // namespace digitalcurling3::players

#endif // DIGITALCURLING3_PLAYERS_PLAYER_IDENTICAL_FACTORY_HPP
