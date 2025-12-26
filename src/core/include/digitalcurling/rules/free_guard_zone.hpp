// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief FreeGuardZoneRule を定義

#pragma once

#include "digitalcurling/rules/i_additional_rule.hpp"

namespace digitalcurling::rules {


/// @brief フリーガードゾーンルール
class FreeGuardZoneRule : public IAdditionalRule {
public:
    /// @brief フリーガードゾーンルールの適用ショット数のデフォルト値
    static constexpr std::uint8_t kDefaultAppliedShotCount = 5;

    /// @brief 適用するショットの数
    std::uint8_t applied_shot_count;

    /// @brief コンストラクタ
    /// @param[in] enabled ルールが有効かどうか
    /// @param[in] applied_shot_count 適用するショットの数
    FreeGuardZoneRule(bool enabled = false, std::uint8_t applied_shot_count = kDefaultAppliedShotCount)
        : IAdditionalRule(enabled), applied_shot_count(applied_shot_count)
        {
            if (!enabled) applied_shot_count = 0;
            else if (applied_shot_count == 0) enabled = false;
        }

    AdditionalRuleTypes GetType() const override { return AdditionalRuleTypes::kFreeGuardZone; }

    bool ValidateShot(std::uint8_t shot_in_end, Team deliver, StoneCoordinate before, StoneCoordinate after) const override {
        if (!is_enabled || applied_shot_count == 0 || shot_in_end > applied_shot_count) return true;

        static auto is_in_free_guard_zone = [](Stone stone) {
            return !stone.IsInHouse() && stone.position.y + Stone::kRadius < coordinate::kTeeLineY;
        };

        Team const opponent = GetOpponentTeam(deliver);
        auto before_stones = before.GetTeamStones(opponent);
        std::vector<StoneIndex> targets {};
        for (std::uint8_t i = 0; i < before_stones.size(); ++i) {
            if (!before_stones[i].has_value()) continue;

            auto stone = before_stones[i].value();
            if (is_in_free_guard_zone(stone)) {
                targets.push_back({opponent, i});
            }
        }

        for (auto const& target : targets) {
            auto stone = after[target];
            if (!stone.has_value() || !is_in_free_guard_zone(stone.value())) {
                return false;
            }
        }
        return true;
    }
};


/// @cond Doxygen_Suppress
//json
inline void to_json(nlohmann::json & j, digitalcurling::rules::FreeGuardZoneRule const& v) {
    j["is_enabled"] = v.is_enabled;
    j["applied_shot_count"] = v.applied_shot_count;
}
inline void from_json(nlohmann::json const& j, digitalcurling::rules::FreeGuardZoneRule & v) {
    j.at("is_enabled").get_to(v.is_enabled);
    try_get_to(j, "applied_shot_count",
        v.applied_shot_count, digitalcurling::rules::FreeGuardZoneRule::kDefaultAppliedShotCount);
}
/// @endcond

} // namespace digitalcurling::rules
