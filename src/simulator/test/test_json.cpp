// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "common.hpp"

namespace dc = digitalcurling;
using nlohmann::json;

TEST(Json, ISimulatorFactoryToJson)
{
    // fcv1
    auto v_fcv1 = std::make_unique<dc::simulators::SimulatorFCV1Factory>();
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
    auto v_fcv1 = dynamic_cast<dc::simulators::SimulatorFCV1Factory*>(v_fcv1_tmp.get());
    if (v_fcv1) {
        EXPECT_EQ(j_fcv1["seconds_per_frame"].get<float>(), v_fcv1->seconds_per_frame);
    } else {
        FAIL();
    }
}
