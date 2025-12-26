// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "common.hpp"

namespace dc = digitalcurling;
using nlohmann::json;

TEST(Json, millisecondsToJson)
{
    std::chrono::milliseconds const v(56'789);
    json const j = v;
    EXPECT_EQ(j.get<std::chrono::milliseconds>(), v);
}

TEST(Json, millisecondsFromJson)
{
    json const j = 1.234;
    std::chrono::milliseconds v;
    j.get_to(v);
    EXPECT_EQ(v.count(), 1234);
}

TEST(Json, Vector2ToJson)
{
    dc::Vector2 const v(1.5f, 2.5f);
    json const j = v;
    EXPECT_EQ(j.at("x").get<float>(), v.x);
    EXPECT_EQ(j.at("y").get<float>(), v.y);
}

TEST(Json, Vector2FromJson)
{
    json const j{
        { "x", 3.5f },
        { "y", 5.5f }
    };
    dc::Vector2 v;
    j.get_to(v);
    EXPECT_EQ(j.at("x").get<float>(), v.x);
    EXPECT_EQ(j.at("y").get<float>(), v.y);
}

TEST(Json, StoneToJson)
{
    dc::Stone const v(dc::Vector2(0.5f, 1.5f), 2.4f);
    json const j = v;
    EXPECT_EQ(j.at("position").at("x").get<float>(), v.position.x);
    EXPECT_EQ(j.at("position").at("y").get<float>(), v.position.y);
    EXPECT_EQ(j.at("angle").get<float>(), v.angle);
}

TEST(Json, StoneFromJson)
{
    json const j{
        { "position", {
            { "x", 1.5f },
            { "y", 2.5f }
        }},
        { "angle", 3.5f }
    };
    dc::Stone v;
    j.get_to(v);
    EXPECT_EQ(j.at("position").at("x").get<float>(), v.position.x);
    EXPECT_EQ(j.at("position").at("y").get<float>(), v.position.y);
    EXPECT_EQ(j.at("angle").get<float>(), v.angle);
}

TEST(Json, MoveToJson)
{
    // shot
    dc::moves::Shot v_shot(1.2f, 1.0f, 1.5f);
    dc::moves::Move v_shot_move = v_shot;
    json const j_shot = v_shot_move;
    EXPECT_EQ(j_shot.at("type").get<std::string>(), "shot");
    EXPECT_EQ(j_shot.at("translational_velocity").get<float>(), v_shot.translational_velocity);
    EXPECT_EQ(j_shot.at("angular_velocity").get<float>(), v_shot.angular_velocity);
    EXPECT_EQ(j_shot.at("release_angle").get<float>(), v_shot.release_angle);

    // concede
    dc::moves::Move const v_concede = dc::moves::Concede();
    json const j_concede = v_concede;
    EXPECT_EQ(j_concede.at("type"), "concede");
}

TEST(Json, MoveFromJson)
{
    // shot
    json const j_shot{
        { "type", "shot" },
        { "translational_velocity", 1.2f },
        { "angular_velocity", 1.0f },
        { "release_angle", 1.5f }
    };
    dc::moves::Move v_shot;
    j_shot.get_to(v_shot);
    if (std::holds_alternative<dc::moves::Shot>(v_shot)) {
        auto const& v_shot2 = std::get<dc::moves::Shot>(v_shot);
        EXPECT_EQ(j_shot.at("translational_velocity").get<float>(), v_shot2.translational_velocity);
        EXPECT_EQ(j_shot.at("angular_velocity").get<float>(), v_shot2.angular_velocity);
        EXPECT_EQ(j_shot.at("release_angle").get<float>(), v_shot2.release_angle);
    } else {
        FAIL();
    }

    // concede
    json const j_concede{
        { "type", "concede" }
    };
    dc::moves::Move v_concede;
    j_concede.get_to(v_concede);
    EXPECT_TRUE(std::holds_alternative<dc::moves::Concede>(v_concede));
}

TEST(Json, TeamToJson)
{
    json const j_team0 = dc::Team::k0;
    json const j_team1 = dc::Team::k1;
    json const j_invalid = dc::Team::kInvalid;
    EXPECT_EQ(j_team0.get<std::string>(), "team0");
    EXPECT_EQ(j_team1.get<std::string>(), "team1");
    EXPECT_TRUE(j_invalid.is_null());
}

TEST(Json, TeamFromJson)
{
    json const j_team0 = "team0";
    json const j_team1 = "team1";
    json const j_invalid = nullptr;
    dc::Team v_team0, v_team1, v_invalid;
    j_team0.get_to(v_team0);
    j_team1.get_to(v_team1);
    j_invalid.get_to(v_invalid);
    EXPECT_EQ(v_team0, dc::Team::k0);
    EXPECT_EQ(v_team1, dc::Team::k1);
    EXPECT_EQ(v_invalid, dc::Team::kInvalid);
}

TEST(Json, GameRuleTypeToJson)
{
    json const j_standard = dc::GameRuleType::kStandard;
    json const j_mixed = dc::GameRuleType::kMixed;
    json const j_mixed_doubles = dc::GameRuleType::kMixedDoubles;
    EXPECT_EQ(j_standard.get<std::string>(), "standard");
    EXPECT_EQ(j_mixed.get<std::string>(), "mixed");
    EXPECT_EQ(j_mixed_doubles.get<std::string>(), "mixed_doubles");
}

TEST(Json, GameRuleTypeFromJson)
{
    json const j_standard = "standard";
    json const j_mixed = "mixed";
    json const j_mixed_doubles = "mixed_doubles";
    dc::GameRuleType v_standard, v_mixed, v_mixed_doubles;
    j_standard.get_to(v_standard);
    j_mixed.get_to(v_mixed);
    j_mixed_doubles.get_to(v_mixed_doubles);
    EXPECT_EQ(v_standard, dc::GameRuleType::kStandard);
    EXPECT_EQ(v_mixed, dc::GameRuleType::kMixed);
    EXPECT_EQ(v_mixed_doubles, dc::GameRuleType::kMixedDoubles);
}

TEST(Json, AdditionalRuleTypesToJson)
{
    json const j_fgz = dc::rules::AdditionalRuleTypes::kFreeGuardZone;
    json const j_nts = dc::rules::AdditionalRuleTypes::kNoTickShot;
    EXPECT_EQ(j_fgz, "free_guard_zone");
    EXPECT_EQ(j_nts, "no_tick_shot");
}

TEST(Json, AdditionalRuleTypesFromJson)
{
    json const j_fgz = "free_guard_zone";
    json const j_nts = "no_tick_shot";
    dc::rules::AdditionalRuleTypes v_fgz;
    dc::rules::AdditionalRuleTypes v_nts;
    j_fgz.get_to(v_fgz);
    j_nts.get_to(v_nts);
    EXPECT_EQ(v_fgz, dc::rules::AdditionalRuleTypes::kFreeGuardZone);
    EXPECT_EQ(v_nts, dc::rules::AdditionalRuleTypes::kNoTickShot);
}

TEST(Json, AdditionalRuleToJson)
{
    dc::rules::FreeGuardZoneRule v_fgz;
    v_fgz.is_enabled = true;
    v_fgz.applied_shot_count = 5;
    json const j = v_fgz;
    EXPECT_EQ(j.at("is_enabled").get<bool>(), v_fgz.is_enabled);
    EXPECT_EQ(j.at("applied_shot_count").get<std::uint8_t>(), v_fgz.applied_shot_count);

    dc::rules::NoTickShotRule v_nts;
    v_nts.is_enabled = false;
    v_nts.applied_shot_count = 5;
    json const j2 = v_nts;
    EXPECT_EQ(j2.at("is_enabled").get<bool>(), v_nts.is_enabled);
    EXPECT_EQ(j2.at("applied_shot_count").get<std::uint8_t>(), v_nts.applied_shot_count);
}

TEST(Json, AdditionalRuleFromJson)
{
    json const j_fgz{
        { "is_enabled", false },
        { "applied_shot_count", 5 }
    };
    dc::rules::FreeGuardZoneRule v_fgz;
    j_fgz.get_to(v_fgz);
    EXPECT_FALSE(v_fgz.is_enabled);
    EXPECT_EQ(v_fgz.applied_shot_count, 5);

    json const j_nts{
        { "is_enabled", true },
        { "applied_shot_count", 5 }
    };
    dc::rules::NoTickShotRule v_nts;
    j_nts.get_to(v_nts);
    EXPECT_TRUE(v_nts.is_enabled);
    EXPECT_EQ(v_nts.applied_shot_count, 5);
}

TEST(Json, GameRuleToJson)
{
    dc::GameRule v;
    v.type = dc::GameRuleType::kStandard;
    v.is_wheelchair = false;
    v.free_guard_zone = dc::rules::FreeGuardZoneRule(true);
    v.no_tick_shot = dc::rules::NoTickShotRule(false);

    json const j = v;
    EXPECT_EQ(j.at("type").get<std::string>(), "standard");
    EXPECT_EQ(j.at("is_wheelchair").get<bool>(), v.is_wheelchair);
    EXPECT_EQ(j.at("free_guard_zone").at("is_enabled").get<bool>(), v.free_guard_zone->is_enabled);
    EXPECT_EQ(j.at("free_guard_zone").at("applied_shot_count").get<std::uint8_t>(), v.free_guard_zone->applied_shot_count);
    EXPECT_EQ(j.at("no_tick_shot").at("is_enabled").get<bool>(), v.no_tick_shot->is_enabled);
    EXPECT_EQ(j.at("no_tick_shot").at("applied_shot_count").get<std::uint8_t>(), v.no_tick_shot->applied_shot_count);
}

TEST(Json, GameRuleFromJson)
{
    json const j{
        { "type", "mixed_doubles" },
        { "is_wheelchair", false }
    };

    dc::GameRule v;
    j.get_to(v);
    EXPECT_EQ(v.type, dc::GameRuleType::kMixedDoubles);
    EXPECT_FALSE(v.is_wheelchair);
    EXPECT_FALSE(v.free_guard_zone.has_value());
    EXPECT_FALSE(v.no_tick_shot.has_value());
}

TEST(Json, GameSettingToJson)
{
    dc::GameSetting v;
    v.max_end = 6;
    v.sheet_width = 5.f;
    v.thinking_time = dc::TeamValue<std::chrono::milliseconds>(
        std::chrono::milliseconds(1'525), std::chrono::milliseconds(23'456)
    );
    v.extra_end_thinking_time = dc::TeamValue<std::chrono::milliseconds>(
        std::chrono::milliseconds(525), std::chrono::milliseconds(3'456)
    );

    json const j = v;
    EXPECT_EQ(j.at("max_end").get<std::uint8_t>(), v.max_end);
    EXPECT_EQ(j.at("sheet_width").get<float>(), v.sheet_width);
    EXPECT_EQ(j.at("thinking_time").at("team0").get<std::chrono::milliseconds>(), v.thinking_time[dc::Team::k0]);
    EXPECT_EQ(j.at("thinking_time").at("team1").get<std::chrono::milliseconds>(), v.thinking_time[dc::Team::k1]);
    EXPECT_EQ(j.at("extra_end_thinking_time").at("team0").get<std::chrono::milliseconds>(), v.extra_end_thinking_time[dc::Team::k0]);
    EXPECT_EQ(j.at("extra_end_thinking_time").at("team1").get<std::chrono::milliseconds>(), v.extra_end_thinking_time[dc::Team::k1]);
}

TEST(Json, GameSettingFromJson)
{
    json const j{
        { "max_end", 10 },
        { "sheet_width", 4.5 },
        { "thinking_time", {
            { "team0", 1.234 },
            { "team1", 5.678 },
        }},
        { "extra_end_thinking_time", {
            { "team0", 0.234 },
            { "team1", 0.678 },
        }}
    };

    dc::GameSetting v;
    j.get_to(v);
    EXPECT_EQ(v.max_end, 10);
    EXPECT_EQ(v.sheet_width, 4.5f);
    EXPECT_EQ(v.thinking_time[dc::Team::k0], std::chrono::milliseconds(1234));
    EXPECT_EQ(v.thinking_time[dc::Team::k1], std::chrono::milliseconds(5678));
    EXPECT_EQ(v.extra_end_thinking_time[dc::Team::k0], std::chrono::milliseconds(234));
    EXPECT_EQ(v.extra_end_thinking_time[dc::Team::k1], std::chrono::milliseconds(678));
}

TEST(Json, GameResultReasonToJson)
{
    json const j_score = dc::GameResult::Reason::kScore;
    json const j_concede = dc::GameResult::Reason::kConcede;
    json const j_time_limit = dc::GameResult::Reason::kTimeLimit;
    json const j_draw = dc::GameResult::Reason::kDraw;
    EXPECT_EQ(j_score.get<std::string>(), "score");
    EXPECT_EQ(j_concede.get<std::string>(), "concede");
    EXPECT_EQ(j_time_limit.get<std::string>(), "time_limit");
    EXPECT_EQ(j_draw.get<std::string>(), "draw");
}

TEST(Json, GameResultReasonFromJson)
{
    json const j_score = "score";
    json const j_concede = "concede";
    json const j_time_limit = "time_limit";
    json const j_draw = "draw";

    dc::GameResult::Reason v_score, v_concede, v_time_limit, v_draw;
    j_score.get_to(v_score);
    j_concede.get_to(v_concede);
    j_time_limit.get_to(v_time_limit);
    j_draw.get_to(v_draw);

    EXPECT_EQ(v_score, dc::GameResult::Reason::kScore);
    EXPECT_EQ(v_concede, dc::GameResult::Reason::kConcede);
    EXPECT_EQ(v_time_limit, dc::GameResult::Reason::kTimeLimit);
    EXPECT_EQ(v_draw, dc::GameResult::Reason::kDraw);
}

TEST(Json, GameResultToJson)
{
    dc::GameResult v;
    v.winner = dc::Team::k1;
    v.reason = dc::GameResult::Reason::kScore;
    json const j = v;
    EXPECT_EQ(j["winner"].get<std::string>(), "team1");
    EXPECT_EQ(j["reason"].get<std::string>(), "score");
}

TEST(Json, GameResultFromJson)
{
    json const j{
        { "winner", nullptr },
        { "reason", "draw" }
    };
    dc::GameResult v;
    j.get_to(v);
    EXPECT_EQ(v.winner, dc::Team::kInvalid);
    EXPECT_EQ(v.reason, dc::GameResult::Reason::kDraw);
}

TEST(Json, GameStateToJson)
{
    dc::GameSetting setting;
    setting.max_end = 10;
    setting.sheet_width = 4.75f;
    setting.thinking_time = dc::TeamValue<std::chrono::milliseconds>(
        std::chrono::milliseconds(1'525), std::chrono::milliseconds(23'456)
    );
    setting.extra_end_thinking_time = dc::TeamValue<std::chrono::milliseconds>(
        std::chrono::milliseconds(525), std::chrono::milliseconds(3'456)
    );

    dc::GameState state(setting);
    state.end = 1;
    state.shot = 10;
    state.hammer = dc::Team::k0;
    state.game_result.reset();

    auto stones0 = std::array<std::optional<dc::Stone>, 8>{};
    auto stones1 = std::array<std::optional<dc::Stone>, 8>{};
    stones0[0] = std::make_optional<dc::Stone>(dc::Vector2(1.5f, 2.5f), 3.5f);
    state.stones = dc::StoneCoordinate(std::array<std::array<std::optional<dc::Stone>, 8>, 2>{ stones0, stones1 });

    auto scores0 = std::vector<std::optional<std::uint8_t>>(setting.max_end + 1, std::nullopt);
    auto scores1 = std::vector<std::optional<std::uint8_t>>(setting.max_end + 1, std::nullopt);
    scores0[0] = 2;
    scores1[0] = 0;
    state.scores = dc::GameScores(std::array<std::vector<std::optional<std::uint8_t>>, 2>{ scores0, scores1 });

    json const j = state;
    EXPECT_EQ(j.at("end").get<std::uint8_t>(), state.end);
    EXPECT_EQ(j.at("shot").get<std::uint8_t>(), state.shot);
    EXPECT_EQ(j.at("hammer").get<std::string>(), "team0");
    EXPECT_EQ(j.at("stones").at("team0")[0].at("position").at("x").get<float>(), 1.5f);
    EXPECT_EQ(j.at("stones").at("team0")[0].at("position").at("y").get<float>(), 2.5f);
    EXPECT_EQ(j.at("stones").at("team0")[0].at("angle").get<float>(), 3.5f);
    EXPECT_TRUE(j.at("stones").at("team1")[0].is_null());
    EXPECT_EQ(j.at("scores").at("team0").size(), setting.max_end + 1);
    EXPECT_EQ(j.at("scores").at("team1").size(), setting.max_end + 1);
    EXPECT_EQ(j.at("scores").at("team0")[0].get<std::uint8_t>(), 2);
    EXPECT_EQ(j.at("scores").at("team1")[0].get<std::uint8_t>(), 0);
    EXPECT_TRUE(j.at("scores").at("team0")[1].is_null());
    EXPECT_TRUE(j.at("scores").at("team1")[1].is_null());
    EXPECT_EQ(j.at("thinking_time_remaining").at("team0").get<std::chrono::milliseconds>(), state.thinking_time_remaining[dc::Team::k0]);
    EXPECT_EQ(j.at("thinking_time_remaining").at("team1").get<std::chrono::milliseconds>(), state.thinking_time_remaining[dc::Team::k1]);
    EXPECT_TRUE(j.at("game_result").is_null());
}

TEST(Json, GameStateFromJson)
{
    // 8 end game
    json const j{
        { "end", 1 },
        { "shot", 3 },
        { "hammer", "team0" },
        { "stones", {
            { "team0", {
                {
                    { "position", {
                        { "x", 1.1f },
                        { "y", 1.2f }
                    }},
                    { "angle", 1.3f }
                },
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr
            }},
            { "team1", {
                {
                    { "position", {
                        { "x", 2.1f },
                        { "y", 2.2f }
                    }},
                    { "angle", 2.3f }
                },
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr
            }}
        }},
        { "scores", {
            { "team0", json::array({
                0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
            })},
            { "team1", json::array({
                3, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
            })}
        }},
        { "thinking_time_remaining", {
            { "team0", 10.5 },
            { "team1", 9.5 }
        }},
        { "game_result", {
            { "winner", "team0" },
            { "reason", "concede" }
        }}
    };

    dc::GameState state;
    j.get_to(state);

    EXPECT_EQ(state.end, 1);
    EXPECT_EQ(state.shot, 3);
    EXPECT_EQ(state.hammer, dc::Team::k0);

    EXPECT_EQ(state.stones[dc::Team::k0][0]->position.x, 1.1f);
    EXPECT_EQ(state.stones[dc::Team::k0][0]->position.y, 1.2f);
    EXPECT_EQ(state.stones[dc::Team::k0][0]->angle, 1.3f);
    EXPECT_FALSE(state.stones[dc::Team::k0][1].has_value());
    EXPECT_EQ(state.stones[dc::Team::k1][0]->position.x, 2.1f);
    EXPECT_EQ(state.stones[dc::Team::k1][0]->position.y, 2.2f);
    EXPECT_EQ(state.stones[dc::Team::k1][0]->angle, 2.3f);
    EXPECT_FALSE(state.stones[dc::Team::k1][1].has_value());

    EXPECT_EQ(state.scores[dc::Team::k0].size(), 9);
    EXPECT_EQ(state.scores[dc::Team::k1].size(), 9);
    EXPECT_EQ(state.scores[dc::Team::k0][0].value(), 0);
    EXPECT_EQ(state.scores[dc::Team::k1][0].value(), 3);
    EXPECT_FALSE(state.scores[dc::Team::k0][1].has_value());
    EXPECT_FALSE(state.scores[dc::Team::k1][1].has_value());

    EXPECT_EQ(state.thinking_time_remaining[dc::Team::k0], std::chrono::milliseconds(10'500));
    EXPECT_EQ(state.thinking_time_remaining[dc::Team::k1], std::chrono::milliseconds(9'500));
    EXPECT_EQ(state.game_result->winner, dc::Team::k0);
    EXPECT_EQ(state.game_result->reason, dc::GameResult::Reason::kConcede);
}
