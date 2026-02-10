// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "common.hpp"

namespace dc = digitalcurling3;
using nlohmann::json;

TEST(Json, IPlayerFactoryToJson)
{
    // normal_dist
    auto v_normal_dist = std::make_unique<dc::players::PlayerNormalDistFactory>();
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
    auto v_identical = std::make_unique<dc::players::PlayerIdenticalFactory>();
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
    auto v_normal_dist = dynamic_cast<dc::players::PlayerNormalDistFactory*>(v_normal_dist_tmp.get());
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
    auto v_nonrandom = dynamic_cast<dc::players::PlayerIdenticalFactory*>(v_nonrandom_tmp.get());
    EXPECT_NE(v_nonrandom, nullptr);
}
