// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "common.hpp"

using digitalcurling::Vector2;
namespace dcco = digitalcurling::coordinate;

namespace {

dcco::Id IndexToId(size_t idx)
{
    switch (idx) {
        case 0:
            return dcco::Id::kSimulation;
        case 1:
            return dcco::Id::kShot0;
        case 2:
            return dcco::Id::kShot1;
    }

    // unexpected
    return dcco::Id::kSimulation;
}

char const * IndexToIdStr(size_t id)
{
    switch (id) {
        case 0:
            return "Simulation";
        case 1:
            return "Shot0";
        case 2:
            return "Shot1";
    }
    return "unexpected";
}

constexpr float kHackY = 21.0315f;

} // unnamed namespace

TEST(Coordinate, TransformPosition)
{
    std::array<Vector2[3], 3> const samples{ {
        // p0
        {
            {0.f, 0.f},    // simulation
            {0.f, kHackY}, // shot0
            {0.f, kHackY}, // shot1
        },

        // p1
        {
            {1.f, -kHackY},       // simulation
            {1.f, 0.f},           // shot0
            {-1.f, kHackY * 2.f}, // shot1
        },

        // p2
        {
            {-1.f, kHackY},       // simulation
            {-1.f, kHackY * 2.f}, // shot0
            {1.f, 0.f},           // shot1
        },
    } };

    // simulation to shot0
    for (size_t i_pos = 0; i_pos < samples.size(); ++i_pos) {
        for (size_t i_from = 0; i_from < 3; ++i_from) {
            for (size_t i_to = 0; i_to < 3; ++i_to) {
                Vector2 result = dcco::TransformPosition(samples[i_pos][i_from], IndexToId(i_from), IndexToId(i_to));
                Vector2 expected = samples[i_pos][i_to];
                EXPECT_FLOAT_EQ(expected.x, result.x) << "transform p" << i_pos << " from " << IndexToIdStr(i_from) << " to " << IndexToIdStr(i_to);
                EXPECT_FLOAT_EQ(expected.y, result.y) << "transform p" << i_pos << " from " << IndexToIdStr(i_from) << " to " << IndexToIdStr(i_to);
            }
        }
    }
}

TEST(Coordinate, TransformVelocity)
{
    Vector2 const sample[3]{
        { 1.f, 1.f },   // simulation
        { 1.f, 1.f },   // shot0
        { -1.f, -1.f }, // shot1
    };
    for (size_t i_from = 0; i_from < 3; ++i_from) {
        for (size_t i_to = 0; i_to < 3; ++i_to) {
            Vector2 result = dcco::TransformVelocity(sample[i_from], IndexToId(i_from), IndexToId(i_to));
            Vector2 expected = sample[i_to];
            EXPECT_FLOAT_EQ(expected.x, result.x) << "transform from " << IndexToIdStr(i_from) << " to " << IndexToIdStr(i_to);
            EXPECT_FLOAT_EQ(expected.y, result.y) << "transform from " << IndexToIdStr(i_from) << " to " << IndexToIdStr(i_to);
        }
    }
}

TEST(Coordinate, TransformAngle)
{
    float const sample[3]{
        1.f,   // simulation
        1.f,   // shot0
        1.f + digitalcurling::kPi, // shot1
    };
    for (size_t i_from = 0; i_from < 3; ++i_from) {
        for (size_t i_to = 0; i_to < 3; ++i_to) {
            float result = dcco::TransformAngle(sample[i_from], IndexToId(i_from), IndexToId(i_to));
            float expected = sample[i_to];
            EXPECT_FLOAT_EQ(expected, result) << "transform from " << IndexToIdStr(i_from) << " to " << IndexToIdStr(i_to);
        }
    }
}

TEST(Coordinate, TransformAngularVelocity)
{
    float const sample[3]{
        1.f,   // simulation
        1.f,   // shot0
        1.f,   // shot1
    };
    for (size_t i_from = 0; i_from < 3; ++i_from) {
        for (size_t i_to = 0; i_to < 3; ++i_to) {
            float result = dcco::TransformAngularVelocity(sample[i_from], IndexToId(i_from), IndexToId(i_to));
            float expected = sample[i_to];
            EXPECT_FLOAT_EQ(expected, result) << "transform from " << IndexToIdStr(i_from) << " to " << IndexToIdStr(i_to);
        }
    }

}

TEST(Coordinate, Positions)
{
    // center line
    EXPECT_FLOAT_EQ(0.f, dcco::GetCenterLineX(dcco::Id::kSimulation)) << "center line on Simulation";
    EXPECT_FLOAT_EQ(0.f, dcco::GetCenterLineX(dcco::Id::kShot0)) << "center line on Shot0";
    EXPECT_FLOAT_EQ(0.f, dcco::GetCenterLineX(dcco::Id::kShot1)) << "center line on Shot1";

    // hog
    EXPECT_FLOAT_EQ(-10.9725f, dcco::GetHogLineY(false, dcco::Id::kSimulation)) << "hog line (0) on Simulation";
    EXPECT_FLOAT_EQ(-10.9725f + kHackY, dcco::GetHogLineY(false, dcco::Id::kShot0)) << "hog line (0) on Shot0";
    EXPECT_FLOAT_EQ(10.9725f + kHackY, dcco::GetHogLineY(false, dcco::Id::kShot1)) << "hog line (0) on Shot1";
    EXPECT_FLOAT_EQ(10.9725f, dcco::GetHogLineY(true, dcco::Id::kSimulation)) << "hog line (1) on Simulation";
    EXPECT_FLOAT_EQ(10.9725f + kHackY, dcco::GetHogLineY(true, dcco::Id::kShot0)) << "hog line (1) on Shot0";
    EXPECT_FLOAT_EQ(-10.9725f + kHackY, dcco::GetHogLineY(true, dcco::Id::kShot1)) << "hog line (1) on Shot1";

    // tee
    EXPECT_FLOAT_EQ(-17.3735f, dcco::GetTeeLineY(false, dcco::Id::kSimulation)) << "tee line (0) on Simulation";
    EXPECT_FLOAT_EQ(-17.3735f + kHackY, dcco::GetTeeLineY(false, dcco::Id::kShot0)) << "tee line (0) on Shot0";
    EXPECT_FLOAT_EQ(17.3735f + kHackY, dcco::GetTeeLineY(false, dcco::Id::kShot1)) << "tee line (0) on Shot1";
    EXPECT_FLOAT_EQ(17.3735f, dcco::GetTeeLineY(true, dcco::Id::kSimulation)) << "tee line (1) on Simulation";
    EXPECT_FLOAT_EQ(17.3735f + kHackY, dcco::GetTeeLineY(true, dcco::Id::kShot0)) << "tee line (1) on Shot0";
    EXPECT_FLOAT_EQ(-17.3735f + kHackY, dcco::GetTeeLineY(true, dcco::Id::kShot1)) << "tee line (1) on Shot1";

    // back
    EXPECT_FLOAT_EQ(-19.2025f, dcco::GetBackLineY(false, dcco::Id::kSimulation)) << "back line (0) on Simulation";
    EXPECT_FLOAT_EQ(-19.2025f + kHackY, dcco::GetBackLineY(false, dcco::Id::kShot0)) << "back line (0) on Shot0";
    EXPECT_FLOAT_EQ(19.2025f + kHackY, dcco::GetBackLineY(false, dcco::Id::kShot1)) << "back line (0) on Shot1";
    EXPECT_FLOAT_EQ(19.2025f, dcco::GetBackLineY(true, dcco::Id::kSimulation)) << "back line (1) on Simulation";
    EXPECT_FLOAT_EQ(19.2025f + kHackY, dcco::GetBackLineY(true, dcco::Id::kShot0)) << "back line (1) on Shot0";
    EXPECT_FLOAT_EQ(-19.2025f + kHackY, dcco::GetBackLineY(true, dcco::Id::kShot1)) << "back line (1) on Shot1";

    // hack
    EXPECT_FLOAT_EQ(-21.0315f, dcco::GetHackY(false, dcco::Id::kSimulation)) << "hack (0) on Simulation";
    EXPECT_FLOAT_EQ(-21.0315f + kHackY, dcco::GetHackY(false, dcco::Id::kShot0)) << "hack (0) on Shot0";
    EXPECT_FLOAT_EQ(21.0315f + kHackY, dcco::GetHackY(false, dcco::Id::kShot1)) << "hack (0) on Shot1";
    EXPECT_FLOAT_EQ(21.0315f, dcco::GetHackY(true, dcco::Id::kSimulation)) << "hack (1) on Simulation";
    EXPECT_FLOAT_EQ(21.0315f + kHackY, dcco::GetHackY(true, dcco::Id::kShot0)) << "hack (1) on Shot0";
    EXPECT_FLOAT_EQ(-21.0315f + kHackY, dcco::GetHackY(true, dcco::Id::kShot1)) << "hack (1) on Shot1";

    // backboard
    EXPECT_FLOAT_EQ(-22.8605f, dcco::GetBackBoardY(false, dcco::Id::kSimulation)) << "backboard (0) on Simulation";
    EXPECT_FLOAT_EQ(-22.8605f + kHackY, dcco::GetBackBoardY(false, dcco::Id::kShot0)) << "backboard (0) on Shot0";
    EXPECT_FLOAT_EQ(22.8605f + kHackY, dcco::GetBackBoardY(false, dcco::Id::kShot1)) << "backboard (0) on Shot1";
    EXPECT_FLOAT_EQ(22.8605f, dcco::GetBackBoardY(true, dcco::Id::kSimulation)) << "backboard (1) on Simulation";
    EXPECT_FLOAT_EQ(22.8605f + kHackY, dcco::GetBackBoardY(true, dcco::Id::kShot0)) << "backboard (1) on Shot0";
    EXPECT_FLOAT_EQ(-22.8605f + kHackY, dcco::GetBackBoardY(true, dcco::Id::kShot1)) << "backboard (1) on Shot1";
}
