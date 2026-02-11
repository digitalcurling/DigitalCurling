// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PlayerNormalDistStorage を定義

#pragma once

#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "digitalcurling/players/gender.hpp"
#include "digitalcurling/players/i_player_storage.hpp"

#include "player_normal_dist_factory.hpp"

namespace digitalcurling::players {


/// @brief プレイヤー NormalDist の正規分布データ
struct NormalDistData {
    /// @brief 速度の分布データ
    std::string speed_dist;
    /// @brief 角度の分布データ
    std::string angle_dist;
};

/// @brief プレイヤー NormalDist のストレージ
class PlayerNormalDistStorage : public IPlayerStorage {
public:
    /// @brief デフォルトコンストラクタ
    PlayerNormalDistStorage() = default;
    /// @brief コピーコンストラクタ
    PlayerNormalDistStorage(PlayerNormalDistStorage const&) = default;
    /// @brief コピー代入演算子
    PlayerNormalDistStorage & operator = (PlayerNormalDistStorage const&) = default;
    virtual ~PlayerNormalDistStorage() override = default;

    virtual const char* GetId() const noexcept override { return DIGITALCURLING_PLUGIN_NAME; }
    virtual nlohmann::json ToJson() const override;

    virtual Gender GetGender() const override { return factory.gender; }
    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;

    /// @brief このストレージに保存されたプレイヤーのファクトリー情報
    PlayerNormalDistFactory factory;
    /// @brief エンジンの状態データ
    std::string engine_data;
    /// @brief 反時計回りのショットに対する正規分布データ
    NormalDistData ccw_data;
    /// @brief 時計回りのショットに対する正規分布データ
    NormalDistData cw_data;
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NormalDistData, speed_dist, angle_dist)

void to_json(nlohmann::json &, PlayerNormalDistStorage const&);
void from_json(nlohmann::json const&, PlayerNormalDistStorage &);
/// @endcond

} // namespace digitalcurling::players
