// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief PlayerIdenticalFactory を定義します

#pragma once

#include <memory>
#include "../json/common.hpp"
#include "../i_player_factory.hpp"

namespace digitalcurling::players {

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

} // namespace digitalcurling::players
