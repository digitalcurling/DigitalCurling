// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief IAdditionalRule を定義

#pragma once

#include <cstdint>
#include "digitalcurling/common.hpp"
#include "digitalcurling/stone_coordinate.hpp"

namespace digitalcurling::rules {


/// @brief 追加ルールの種類
enum class AdditionalRuleTypes {
    /// @brief フリーガードゾーン
    kFreeGuardZone,
    /// @brief ノーティックショット
    kNoTickShot,
};

/// @brief 追加ルールのインターフェース
class IAdditionalRule {
public:
    /// @brief ルールが有効かどうか
    bool is_enabled;

    /// @brief コンストラクタ
    /// @param[in] enabled ルールが有効かどうか
    IAdditionalRule(bool enabled = false) : is_enabled(enabled) {}
    virtual ~IAdditionalRule() = default;

    /// @brief ルールの種類を取得する
    /// @return ルールの種類
    virtual AdditionalRuleTypes GetType() const = 0;

    /// @brief ショットがルールに従っているかを検証する
    /// @param[in] shot_in_end エンド内のショット番号
    /// @param[in] deliver ショットしたチーム
    /// @param[in] before ショット前の盤面情報
    /// @param[in] after ショット後の盤面情報
    /// @return ショットがルールに従っている場合は `true` そうでない場合は `false`
    virtual bool ValidateShot(std::uint8_t shot_in_end, Team deliver, StoneCoordinate before, StoneCoordinate after) const = 0;
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_JSON_SERIALIZE_ENUM(AdditionalRuleTypes, {
    { AdditionalRuleTypes::kFreeGuardZone, "free_guard_zone" },
    { AdditionalRuleTypes::kNoTickShot, "no_tick_shot" },
});
/// @endcond

} // namespace digitalcurling::rules
