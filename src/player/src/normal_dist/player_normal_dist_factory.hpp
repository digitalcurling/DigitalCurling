// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PlayerNormalDistFactory を定義

#pragma once

#include <random>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include "digitalcurling/players/gender.hpp"
#include "digitalcurling/players/i_player_factory.hpp"

#include "normal_dist.hpp"

namespace digitalcurling::players {


/// @brief プレイヤー NormalDist のパラメータ
struct PlayerNormalDistParameter {
    /// @brief ショットの最大速度
    float max_speed = 4.f;
    /// @brief ショットの初速に加わる正規分布乱数の標準偏差
    float stddev_speed = 0.0076f;
    /// @brief ショットの初期角度に加わる正規分布乱数の標準偏差
    float stddev_angle = 0.0018f;
};

/// @brief プレイヤー NormalDist のファクトリー
class PlayerNormalDistFactory : public IPlayerFactory {
public:
    /// @brief プレイヤーの性別
    Gender gender = Gender::kUnknown;

    /// @brief 時計回りのショットに対するパラメータ
    PlayerNormalDistParameter cw = {};

    /// @brief 反時計回りのショットに対するパラメータ
    PlayerNormalDistParameter ccw = {};

    /// @brief 乱数のシード値
    ///
    /// `std::nullopt` の場合シード値を自動でランダムに設定します。
    std::optional<std::random_device::result_type> seed = std::nullopt;

    /// @brief デフォルトコンストラクタ
    PlayerNormalDistFactory() = default;
    /// @brief コピーコンストラクタ
    PlayerNormalDistFactory(PlayerNormalDistFactory const&) = default;
    /// @brief コピー代入演算子
    PlayerNormalDistFactory & operator = (PlayerNormalDistFactory const&) = default;
    virtual ~PlayerNormalDistFactory() override = default;

    virtual const char* GetId() const noexcept override { return DIGITALCURLING_PLUGIN_NAME; }
    virtual nlohmann::json ToJson() const override;

    virtual Gender GetGender() const override { return gender; }
    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;
    virtual std::unique_ptr<IPlayerFactory> Clone() const override;
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerNormalDistParameter, max_speed, stddev_speed, stddev_angle)

void to_json(nlohmann::json &, PlayerNormalDistFactory const&);
void from_json(nlohmann::json const&, PlayerNormalDistFactory &);
/// @endcond

} // namespace digitalcurling::players
