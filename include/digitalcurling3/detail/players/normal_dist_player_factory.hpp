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
/// \brief NormalDistPlayerFactory を定義します

#ifndef DIGITALCURLING3_PLAYERS_NORMAL_DIST_PLAYER_FACTORY_HPP
#define DIGITALCURLING3_PLAYERS_NORMAL_DIST_PLAYER_FACTORY_HPP

#include <random>
#include <memory>
#include <optional>
#include "../json/common.hpp"
#include "../i_player_factory.hpp"

namespace digitalcurling3::players {

/// \brief Normal DistプレイヤーのID
constexpr std::string_view kNormalDistPlayerId = "normal_dist";


/// \brief ショットの初速と角度に正規分布の乱数を加えるプレイヤー
class NormalDistPlayerFactory : public IPlayerFactory {
public:
    /// \brief ショットの最大速度
    float max_speed = 4.f;

    /// \brief ショットの初速に加わる正規分布乱数の標準偏差．
    float stddev_speed = 0.0076f;

    /// \brief ショットの初期角度に加わる正規分布乱数の標準偏差．
    float stddev_angle = 0.0018f;

    /// \brief 乱数のシード値．
    ///
    /// \c std::nullopt の場合シード値を自動でランダムに選択する．
    std::optional<std::random_device::result_type> seed = std::nullopt;

    NormalDistPlayerFactory() = default;  ///< デフォルトコンストラクタ
    virtual ~NormalDistPlayerFactory() = default;
    NormalDistPlayerFactory(NormalDistPlayerFactory const&) = default;  ///< コピーコンストラクタ
    NormalDistPlayerFactory & operator = (NormalDistPlayerFactory const&) = default;  ///< コピー代入演算子

    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;
    virtual std::unique_ptr<IPlayerFactory> Clone() const override;
    virtual std::string GetPlayerId() const override
    {
        return std::string(kNormalDistPlayerId);
    }
};


/// \cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, NormalDistPlayerFactory const&);
void from_json(nlohmann::json const&, NormalDistPlayerFactory &);
/// \endcond

} // namespace digitalcurling3::players

#endif // DIGITALCURLING3_PLAYERS_NORMAL_DIST_PLAYER_FACTORY_HPP
