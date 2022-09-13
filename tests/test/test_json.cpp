﻿#include "common.hpp"

namespace dc = digital_curling;
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

TEST(Json, TransformToJson)
{
    dc::Transform const v(
        dc::Vector2(0.5f, 1.5f),
        2.4f);
    json const j = v;
    EXPECT_EQ(j.at("position").at("x").get<float>(), v.position.x);
    EXPECT_EQ(j.at("position").at("y").get<float>(), v.position.y);
    EXPECT_EQ(j.at("angle").get<float>(), v.angle);
}

TEST(Json, TransformFromJson)
{
    json const j{
        { "position", {
            { "x", 1.5f },
            { "y", 2.5f }
        }},
        { "angle", 3.5f }
    };
    dc::Transform v;
    j.get_to(v);
    EXPECT_EQ(j.at("position").at("x").get<float>(), v.position.x);
    EXPECT_EQ(j.at("position").at("y").get<float>(), v.position.y);
    EXPECT_EQ(j.at("angle").get<float>(), v.angle);
}

TEST(Json, ShotRotationToJson)
{
    json const j_ccw = dc::moves::Shot::Rotation::kCCW;
    json const j_cw = dc::moves::Shot::Rotation::kCW;
    EXPECT_EQ(j_ccw, "ccw");
    EXPECT_EQ(j_cw, "cw");
}

TEST(Json, ShotRotationFromJson)
{
    json const j_ccw = "ccw";
    json const j_cw = "cw";
    dc::moves::Shot::Rotation v_ccw;
    dc::moves::Shot::Rotation v_cw;
    j_ccw.get_to(v_ccw);
    j_cw.get_to(v_cw);
    EXPECT_EQ(v_ccw, dc::moves::Shot::Rotation::kCCW);
    EXPECT_EQ(v_cw, dc::moves::Shot::Rotation::kCW);
}

TEST(Json, MoveToJson)
{
    // shot
    dc::moves::Shot v_shot{ dc::Vector2(0.1f, 0.2f), dc::moves::Shot::Rotation::kCW };
    dc::Move v_shot_move = v_shot;
    json const j_shot = v_shot_move;
    EXPECT_EQ(j_shot.at("type").get<std::string>(), "shot");
    EXPECT_EQ(j_shot.at("velocity").at("x").get<float>(), v_shot.velocity.x);
    EXPECT_EQ(j_shot.at("velocity").at("y").get<float>(), v_shot.velocity.y);
    EXPECT_EQ(j_shot.at("rotation").get<dc::moves::Shot::Rotation>(), v_shot.rotation);

    // concede
    dc::Move const v_concede = dc::moves::Concede();
    json const j_concede = v_concede;
    EXPECT_EQ(j_concede.at("type"), "concede");
}

TEST(Json, MoveFromJson)
{
    // shot
    json const j_shot{
        { "type", "shot" },
        { "velocity", {
            { "x", 0.2f },
            { "y", 1.2f }
        }},
        { "rotation", "ccw" }
    };
    dc::Move v_shot;
    j_shot.get_to(v_shot);
    if (std::holds_alternative<dc::moves::Shot>(v_shot)) {
        auto const& v_shot2 = std::get<dc::moves::Shot>(v_shot);
        EXPECT_EQ(j_shot.at("velocity").at("x").get<float>(), v_shot2.velocity.x);
        EXPECT_EQ(j_shot.at("velocity").at("y").get<float>(), v_shot2.velocity.y);
        EXPECT_EQ(j_shot.at("rotation").get<dc::moves::Shot::Rotation>(), v_shot2.rotation);
    } else {
        FAIL();
    }

    // concede
    json const j_concede{
        { "type", "concede" }
    };
    dc::Move v_concede;
    j_concede.get_to(v_concede);
    EXPECT_TRUE(std::holds_alternative<dc::moves::Concede>(v_concede));
}

TEST(Json, MoveResultToJson)
{
    dc::ApplyMoveResult v;
    v.free_guard_zone_foul = false;

    json const j = v;
    EXPECT_FALSE(j["free_guard_zone_foul"].get<bool>());
}

TEST(Json, MoveResultFromJson)
{
    json const j{
        { "free_guard_zone_foul", true }
    };

    dc::ApplyMoveResult v;
    j.get_to(v);

    EXPECT_TRUE(v.free_guard_zone_foul);
}

TEST(Json, IPlayerFactoryToJson)
{
    // normal_dist
    auto v_normal_dist = std::make_unique<dc::players::NormalDistPlayerFactory>();
    v_normal_dist->max_speed = 5.0f;
    v_normal_dist->stddev_speed = 0.1f;
    v_normal_dist->stddev_angle = 0.2f;
    v_normal_dist->seed = 3;
    json const j_normal_dist = static_cast<dc::IPlayerFactory&>(*v_normal_dist.get());
    EXPECT_EQ(j_normal_dist.at("type").get<std::string>(), "normal_dist");
    EXPECT_EQ(j_normal_dist.at("max_speed").get<float>(), v_normal_dist->max_speed);
    EXPECT_EQ(j_normal_dist.at("stddev_speed").get<float>(), v_normal_dist->stddev_speed);
    EXPECT_EQ(j_normal_dist.at("stddev_angle").get<float>(), v_normal_dist->stddev_angle);
    EXPECT_EQ(j_normal_dist.at("seed").get<std::random_device::result_type>(), v_normal_dist->seed.value());

    // identical
    auto v_identical = std::make_unique<dc::players::IdenticalPlayerFactory>();
    json const j_identical = static_cast<dc::IPlayerFactory&>(*v_identical.get());
    EXPECT_EQ(j_identical.at("type").get<std::string>(), "identical");
}

TEST(Json, IPlayerFactoryFromJson)
{
    // normal_dist
    json const j_normal_dist{
        { "type", "normal_dist" },
        { "max_speed", 8.f },
        { "stddev_speed", 0.1f },
        { "stddev_angle", 0.2f },
        { "seed", 4 }
    };

    auto v_normal_dist_tmp = j_normal_dist.get<std::unique_ptr<dc::IPlayerFactory>>();
    auto v_normal_dist = dynamic_cast<dc::players::NormalDistPlayerFactory*>(v_normal_dist_tmp.get());
    if (v_normal_dist) {
        EXPECT_EQ(j_normal_dist["max_speed"].get<float>(), v_normal_dist->max_speed);
        EXPECT_EQ(j_normal_dist["stddev_speed"].get<float>(), v_normal_dist->stddev_speed);
        EXPECT_EQ(j_normal_dist["stddev_angle"].get<float>(), v_normal_dist->stddev_angle);
        EXPECT_EQ(j_normal_dist["seed"].get<std::random_device::result_type>(), v_normal_dist->seed.value());
    } else {
        FAIL();
    }

    // nonrandom
    json const j_nonrandom{
        { "type", "identical" }
    };
    auto v_nonrandom_tmp = j_nonrandom.get<std::unique_ptr<dc::IPlayerFactory>>();
    auto v_nonrandom = dynamic_cast<dc::players::IdenticalPlayerFactory*>(v_nonrandom_tmp.get());
    EXPECT_NE(v_nonrandom, nullptr);
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

TEST(Json, GameSettingToJson)
{
    dc::GameSetting v;
    v.max_end = 6;
    v.sheet_width = 5.f;
    v.five_rock_rule = true;
    v.thinking_time[0] = std::chrono::milliseconds(1'525);
    v.thinking_time[1] = std::chrono::milliseconds(23'456);
    v.extra_end_thinking_time[0] = std::chrono::milliseconds(525);
    v.extra_end_thinking_time[1] = std::chrono::milliseconds(3'456);
    json const j = v;
    EXPECT_EQ(j.at("max_end").get<decltype(dc::GameSetting::max_end)>(), v.max_end);
    EXPECT_EQ(j.at("sheet_width").get<decltype(dc::GameSetting::sheet_width)>(), v.sheet_width);
    EXPECT_EQ(j.at("five_rock_rule").get<decltype(dc::GameSetting::five_rock_rule)>(), v.five_rock_rule);
    EXPECT_EQ(j.at("thinking_time").at("team0").get<decltype(dc::GameSetting::thinking_time)::value_type>(), v.thinking_time[0]);
    EXPECT_EQ(j.at("thinking_time").at("team1").get<decltype(dc::GameSetting::thinking_time)::value_type>(), v.thinking_time[1]);
    EXPECT_EQ(j.at("extra_end_thinking_time").at("team0").get<decltype(dc::GameSetting::thinking_time)::value_type>(), v.extra_end_thinking_time[0]);
    EXPECT_EQ(j.at("extra_end_thinking_time").at("team1").get<decltype(dc::GameSetting::thinking_time)::value_type>(), v.extra_end_thinking_time[1]);
}

TEST(Json, GameSettingFromJson)
{
    json const j{
        { "max_end", 10 },
        { "sheet_width", 4.5 },
        { "five_rock_rule", false },
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
    EXPECT_FALSE(v.five_rock_rule);
    EXPECT_EQ(v.thinking_time[0], std::chrono::milliseconds(1234));
    EXPECT_EQ(v.thinking_time[1], std::chrono::milliseconds(5678));
    EXPECT_EQ(v.extra_end_thinking_time[0], std::chrono::milliseconds(234));
    EXPECT_EQ(v.extra_end_thinking_time[1], std::chrono::milliseconds(678));
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
    setting.five_rock_rule = true;
    setting.thinking_time[0] = std::chrono::milliseconds(10'000);
    setting.thinking_time[1] = std::chrono::milliseconds(10'000);
    setting.extra_end_thinking_time[0] = std::chrono::milliseconds(1'000);
    setting.extra_end_thinking_time[1] = std::chrono::milliseconds(1'000);


    dc::GameState state(setting);
    state.end = 1;
    state.shot = 10;
    state.hammer = dc::Team::k0;
    state.stones[0][0].emplace(dc::Vector2(0.5f, 0.1f), 0.2f);
    state.scores[0][0].emplace(2);
    state.scores[1][0].emplace(0);
    state.extra_end_score[0].reset();
    state.extra_end_score[1].reset();
    state.game_result.reset();

    json const j = state;
    EXPECT_EQ(j["end"].get<std::uint8_t>(), state.end);
    EXPECT_EQ(j["shot"].get<std::uint8_t>(), state.shot);
    EXPECT_EQ(j["hammer"].get<std::string>(), "team0");
    EXPECT_EQ(j["stones"]["team0"][0]["position"]["x"].get<float>(), 0.5f);
    EXPECT_TRUE(j["stones"]["team1"][0].is_null());
    EXPECT_EQ(j["scores"]["team0"].size(), setting.max_end);
    EXPECT_EQ(j["scores"]["team1"].size(), setting.max_end);
    EXPECT_EQ(j["scores"]["team0"][0].get<std::uint8_t>(), 2);
    EXPECT_EQ(j["scores"]["team1"][0].get<std::uint8_t>(), 0);
    EXPECT_TRUE(j["scores"]["team0"][1].is_null());
    EXPECT_TRUE(j["scores"]["team1"][1].is_null());
    EXPECT_EQ(j["thinking_time_remaining"]["team0"].get<std::chrono::milliseconds>(), state.thinking_time_remaining[0]);
    EXPECT_EQ(j["thinking_time_remaining"]["team1"].get<std::chrono::milliseconds>(), state.thinking_time_remaining[1]);
    EXPECT_TRUE(j["extra_end_score"]["team0"].is_null());
    EXPECT_TRUE(j["extra_end_score"]["team1"].is_null());
    EXPECT_TRUE(j["game_result"].is_null());
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
                0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
            })},
            { "team1", json::array({
                3, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
            })}
        }},
        { "extra_end_score", {
            { "team0", nullptr },
            { "team1", nullptr }
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
    EXPECT_EQ(state.stones[0][0]->position.x, 1.1f);
    EXPECT_EQ(state.stones[0][0]->position.y, 1.2f);
    EXPECT_EQ(state.stones[0][0]->angle, 1.3f);
    EXPECT_FALSE(state.stones[0][1].has_value());
    EXPECT_EQ(state.stones[1][0]->position.x, 2.1f);
    EXPECT_EQ(state.stones[1][0]->position.y, 2.2f);
    EXPECT_EQ(state.stones[1][0]->angle, 2.3f);
    EXPECT_FALSE(state.stones[1][1].has_value());
    EXPECT_EQ(state.scores[0].size(), 8);
    EXPECT_EQ(state.scores[1].size(), 8);
    EXPECT_EQ(state.scores[0][0].value(), 0);
    EXPECT_EQ(state.scores[1][0].value(), 3);
    EXPECT_FALSE(state.scores[0][1].has_value());
    EXPECT_FALSE(state.scores[1][1].has_value());
    EXPECT_FALSE(state.extra_end_score[0].has_value());
    EXPECT_FALSE(state.extra_end_score[1].has_value());
    EXPECT_EQ(state.thinking_time_remaining[0], std::chrono::milliseconds(10'500));
    EXPECT_EQ(state.thinking_time_remaining[1], std::chrono::milliseconds(9'500));
    EXPECT_EQ(state.game_result->winner, dc::Team::k0);
    EXPECT_EQ(state.game_result->reason, dc::GameResult::Reason::kConcede);
}

TEST(Json, ISimulatorFactoryToJson)
{
    // fcv1
    auto v_fcv1 = std::make_unique<dc::simulators::FCV1SimulatorFactory>();
    v_fcv1->seconds_per_frame = 0.24f;

    json const j_fcv1 = static_cast<dc::ISimulatorFactory&>(*v_fcv1.get());

    EXPECT_EQ(j_fcv1["type"].get<std::string>(), "fcv1");
    EXPECT_EQ(j_fcv1["seconds_per_frame"].get<float>(), v_fcv1->seconds_per_frame);
}

TEST(Json, ISimulatorFactoryFromJson)
{
    // fcv1
    json const j_fcv1{
        { "type", "fcv1" },
        { "seconds_per_frame", 0.25f }
    };
    auto v_fcv1_tmp = j_fcv1.get<std::unique_ptr<dc::ISimulatorFactory>>();
    auto v_fcv1 = dynamic_cast<dc::simulators::FCV1SimulatorFactory*>(v_fcv1_tmp.get());
    if (v_fcv1) {
        EXPECT_EQ(j_fcv1["seconds_per_frame"].get<float>(), v_fcv1->seconds_per_frame);
    } else {
        FAIL();
    }
}


