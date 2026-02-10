// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief PlayerNormalDistFactory を定義します

#pragma once

#include <random>
#include <memory>
#include <optional>
#include "../json/common.hpp"
#include "../i_player_factory.hpp"

namespace digitalcurling3::players {

/// \brief プレイヤーNormal DistのID
constexpr std::string_view kPlayerNormalDistId = "normal_dist";


/// \brief ショットの初速に速度上限を適用したのち，初速と角度に正規分布の乱数を加えるプレイヤー
class PlayerNormalDistFactory : public IPlayerFactory {
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

    PlayerNormalDistFactory() = default;  ///< デフォルトコンストラクタ
    virtual ~PlayerNormalDistFactory() = default;
    PlayerNormalDistFactory(PlayerNormalDistFactory const&) = default;  ///< コピーコンストラクタ
    PlayerNormalDistFactory & operator = (PlayerNormalDistFactory const&) = default;  ///< コピー代入演算子

    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;
    virtual std::unique_ptr<IPlayerFactory> Clone() const override;
    virtual std::string GetPlayerId() const override
    {
        return std::string(kPlayerNormalDistId);
    }
};


/// \cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, PlayerNormalDistFactory const&);
void from_json(nlohmann::json const&, PlayerNormalDistFactory &);
/// \endcond

} // namespace digitalcurling3::players
