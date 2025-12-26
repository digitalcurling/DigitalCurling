// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief GameRuleType, GameRule を定義

#pragma once

#include <cstdint>
#include <optional>
#include <utility>
#include "digitalcurling/common.hpp"
#include "digitalcurling/team.hpp"
#include "digitalcurling/rules/i_additional_rule.hpp"
#include "digitalcurling/rules/free_guard_zone.hpp"
#include "digitalcurling/rules/no_tick_shot.hpp"

namespace digitalcurling {

class StoneCoordinate;

/// @brief 試合のルールの種類
enum class GameRuleType {
    /// @brief 通常(4人制)ルール
    kStandard,
    /// @brief ミックスルール
    kMixed,
    /// @brief ミックスダブルスルール
    kMixedDoubles
};

/// @brief 試合のルール
struct GameRule {
public:
    /// @brief ゲームのルールの種類
    GameRuleType type;

    /// @brief 車いすルールを適用するか
    ///! @note 未実装
    bool is_wheelchair = false;

    /// @brief フリーガードゾーンルール
    std::optional<rules::FreeGuardZoneRule> free_guard_zone;

    /// @brief ノーティックショットルール
    std::optional<rules::NoTickShotRule> no_tick_shot;


    /// @brief ショットがルールに従っているかを検証する
    /// @param[in] shot_in_end エンド内のショット番号
    /// @param[in] deliver ショットしたチーム
    /// @param[in] before ショット前の盤面情報
    /// @param[in] after ショット後の盤面情報
    /// @return 違反したルールの種類 (ルールに従っている場合は `std::nullopt`)
    std::optional<rules::AdditionalRuleTypes> VerifyShot(std::uint8_t shot_in_end, Team deliver, StoneCoordinate before, StoneCoordinate after) const {
        std::pair<rules::AdditionalRuleTypes, const rules::IAdditionalRule*> rules[2] = {
            std::make_pair(rules::AdditionalRuleTypes::kFreeGuardZone, free_guard_zone.has_value() ? &(*free_guard_zone) : nullptr),
            std::make_pair(rules::AdditionalRuleTypes::kNoTickShot, no_tick_shot.has_value() ? &(*no_tick_shot) : nullptr)
        };

        for (auto const& rule : rules) {
            if (rule.second && rule.second->is_enabled) {
                if (!rule.second->ValidateShot(shot_in_end, deliver, before, after)) {
                    return std::make_optional(rule.first);
                }
            }
        }
        return std::nullopt;
    }
};


/// @cond Doxygen_Suppress
//json
NLOHMANN_JSON_SERIALIZE_ENUM(GameRuleType, {
    { GameRuleType::kStandard, "standard" },
    { GameRuleType::kMixed, "mixed" },
    { GameRuleType::kMixedDoubles, "mixed_doubles" },
});

inline void to_json(nlohmann::json & j, GameRule const& v) {
    j["type"] = v.type;
    j["is_wheelchair"] = v.is_wheelchair;
    j["free_guard_zone"] = v.free_guard_zone;
    j["no_tick_shot"] = v.no_tick_shot;
};
inline void from_json(nlohmann::json const& j, GameRule & v) {
    j.at("type").get_to(v.type);
    j.at("is_wheelchair").get_to(v.is_wheelchair);
    try_get_to(j, "free_guard_zone", v.free_guard_zone, std::nullopt);
    try_get_to(j, "no_tick_shot", v.no_tick_shot, std::nullopt);
}
/// @endcond

} // namespace digitalcurling
