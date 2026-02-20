#include <memory>
#include <string>
#include "common.hpp"
#include "../src/normal_dist/player_normal_dist.hpp"
#include "../src/normal_dist/player_normal_dist_factory.hpp"

namespace dct = digitalcurling::test;

TEST(PlayerNormalDist, SaveLoad1)
{
    dcp::PlayerNormalDistFactory factory;
    dct::PlayerTestSaveLoad1(factory.CreatePlayer());
}

TEST(PlayerNormalDist, SaveLoad2)
{
    dcp::PlayerNormalDistFactory factory;
    dct::PlayerTestSaveLoad2(factory.CreatePlayer());
}

TEST(PlayerNormalDist, SaveLoad3)
{
    dcp::PlayerNormalDistFactory factory;
    dct::PlayerTestSaveLoad3(factory.CreatePlayer());
}

TEST(PlayerNormalDist, SaveLoad4)
{
    dcp::PlayerNormalDistFactory factory;
    dct::PlayerTestSaveLoad4<dcp::PlayerNormalDistStorage>(factory.CreatePlayer());
}


TEST(PlayerNormalDist, FactoryToJson)
{
    auto v_normal_dist = std::make_unique<dcp::PlayerNormalDistFactory>();
    v_normal_dist->cw.max_speed = 5.0f;
    v_normal_dist->cw.stddev_speed = 0.1f;
    v_normal_dist->cw.stddev_angle = 0.2f;
    v_normal_dist->ccw.max_speed = 6.0f;
    v_normal_dist->ccw.stddev_speed = 0.11f;
    v_normal_dist->ccw.stddev_angle = 0.21f;
    v_normal_dist->seed = 3;

    json const j_normal_dist = *v_normal_dist.get();
    EXPECT_EQ(j_normal_dist.at("type").get<std::string>(), "normal_dist");
}

TEST(PlayerNormalDist, FactoryFromJson1)
{
    json const j_normal_dist{
        { "type", "normal_dist" },
        { "gender", "female" },
        { "max_speed", 8.f },
        { "stddev_speed", 0.1f },
        { "stddev_angle", 0.2f },
        { "seed", 4 }
    };

    dcp::PlayerNormalDistFactory v_normal_dist;
    EXPECT_NO_THROW(v_normal_dist = j_normal_dist.get<dcp::PlayerNormalDistFactory>());
    EXPECT_EQ(j_normal_dist.at("gender").get<dcp::Gender>(), v_normal_dist.gender);
    EXPECT_EQ(j_normal_dist.at("seed").get<std::random_device::result_type>(), v_normal_dist.seed.value());

    float max_speed = j_normal_dist.at("max_speed").get<float>();
    EXPECT_EQ(max_speed, v_normal_dist.cw.max_speed);
    EXPECT_EQ(max_speed, v_normal_dist.ccw.max_speed);

    float stddev_speed = j_normal_dist.at("stddev_speed").get<float>();
    EXPECT_EQ(stddev_speed, v_normal_dist.cw.stddev_speed);
    EXPECT_EQ(stddev_speed, v_normal_dist.ccw.stddev_speed);

    float stddev_angle = j_normal_dist.at("stddev_angle").get<float>();
    EXPECT_EQ(stddev_angle, v_normal_dist.cw.stddev_angle);
    EXPECT_EQ(stddev_angle, v_normal_dist.ccw.stddev_angle);
}

TEST(PlayerNormalDist, FactoryFromJson2)
{
    json const j_normal_dist{
        { "type", "normal_dist" },
        { "gender", "male" },
        { "cw", {
            { "max_speed", 7.f },
            { "stddev_speed", 0.11f },
            { "stddev_angle", 0.21f }
        }},
        { "ccw", {
            { "max_speed", 6.f },
            { "stddev_speed", 0.12f },
            { "stddev_angle", 0.22f }
        }},
        { "seed", 5 }
    };

    dcp::PlayerNormalDistFactory v_normal_dist;
    EXPECT_NO_THROW(v_normal_dist = j_normal_dist.get<dcp::PlayerNormalDistFactory>());
    EXPECT_EQ(j_normal_dist.at("gender").get<dcp::Gender>(), v_normal_dist.gender);
    EXPECT_EQ(j_normal_dist.at("seed").get<std::random_device::result_type>(), v_normal_dist.seed.value());

    auto const& j_cw = j_normal_dist.at("cw");
    EXPECT_EQ(j_cw.at("max_speed").get<float>(), v_normal_dist.cw.max_speed);
    EXPECT_EQ(j_cw.at("stddev_speed").get<float>(), v_normal_dist.cw.stddev_speed);
    EXPECT_EQ(j_cw.at("stddev_angle").get<float>(), v_normal_dist.cw.stddev_angle);

    auto const& j_ccw = j_normal_dist.at("ccw");
    EXPECT_EQ(j_ccw.at("max_speed").get<float>(), v_normal_dist.ccw.max_speed);
    EXPECT_EQ(j_ccw.at("stddev_speed").get<float>(), v_normal_dist.ccw.stddev_speed);
    EXPECT_EQ(j_ccw.at("stddev_angle").get<float>(), v_normal_dist.ccw.stddev_angle);
}