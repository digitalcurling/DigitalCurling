// Copyright (c) 2022-2025 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PlayerIdenticalFactory を定義

#pragma once

#include <memory>
#include "digitalcurling/common.hpp"
#include "digitalcurling/players/i_player_factory.hpp"

#include "identical.hpp"

namespace digitalcurling::players {


/// @brief プレイヤー Identical のファクトリー
class PlayerIdenticalFactory : public IPlayerFactory {
public:
    /// @brief プレイヤーの性別
    Gender gender = Gender::kUnknown;

    /// @brief デフォルトコンストラクタ
    PlayerIdenticalFactory() = default;
    /// @brief コピーコンストラクタ
    PlayerIdenticalFactory(PlayerIdenticalFactory const&) = default;
    /// @brief コピー代入演算子
    PlayerIdenticalFactory & operator = (PlayerIdenticalFactory const&) = default;
    virtual ~PlayerIdenticalFactory() override = default;

    virtual const char* GetId() const noexcept override { return DIGITALCURLING_PLUGIN_NAME; }
    virtual nlohmann::json ToJson() const override;

    virtual Gender GetGender() const override { return gender; }
    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;
    virtual std::unique_ptr<IPlayerFactory> Clone() const override;
};


/// @cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, PlayerIdenticalFactory const&);
void from_json(nlohmann::json const&, PlayerIdenticalFactory &);
/// @endcond

} // namespace digitalcurling::players
