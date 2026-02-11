#include <memory>
#include <string>
#include "common.hpp"
#include "../src/fcv1/simulator_fcv1_factory.hpp"
#include "../src/fcv1/simulator_fcv1_storage.hpp"

namespace dct = digitalcurling::test;

TEST(SimulatorFCV1, InitialState)
{
    dcs::SimulatorFCV1Factory factory;
    auto simulator = factory.CreateSimulator();
    EXPECT_TRUE(simulator->AreAllStonesStopped());
}

TEST(SimulatorFCV1, SaveLoad1)
{
    dcs::SimulatorFCV1Factory factory;
    auto simulator = factory.CreateSimulator();

    {
        dcs::ISimulator::AllStones init_stones;
        init_stones[0] = dcs::ISimulator::StoneState(dc::Vector2(0.1f, 2.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        init_stones[15] = dcs::ISimulator::StoneState(dc::Vector2(0.1f, -4.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        simulator->SetStones(init_stones);
    }

    // 1
    auto storage = simulator->CreateStorage();  // create & save
    for (int i = 0; i < 10; ++i) { simulator->Step(); }
    auto const stones1 = simulator->GetStones();

    // 2
    simulator->Load(*storage);
    for (int i = 0; i < 10; ++i) { simulator->Step(); }
    auto const stones2 = simulator->GetStones();

    EXPECT_TRUE(dct::EqualsSimulatorStones(stones1, stones2));
}

TEST(SimulatorFCV1, SaveLoad2)
{
    dcs::SimulatorFCV1Factory factory;
    auto simulator = factory.CreateSimulator();
    auto storage = simulator->CreateStorage();  // create & save

    {
        dcs::ISimulator::AllStones init_stones;
        init_stones[0] = dcs::ISimulator::StoneState(dc::Vector2(0.1f, 2.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        init_stones[15] = dcs::ISimulator::StoneState(dc::Vector2(0.1f, -4.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        simulator->SetStones(init_stones);
    }

    // 1
    simulator->Save(*storage);
    for (int i = 0; i < 10; ++i) { simulator->Step(); }
    auto const stones1 = simulator->GetStones();

    // 2
    simulator->Load(*storage);
    for (int i = 0; i < 10; ++i) { simulator->Step(); }
    auto const stones2 = simulator->GetStones();

    EXPECT_TRUE(dct::EqualsSimulatorStones(stones1, stones2));
}

TEST(SimulatorFCV1, SaveLoad3)
{
    dcs::SimulatorFCV1Factory factory;
    auto simulator = factory.CreateSimulator();

    {
        dcs::ISimulator::AllStones init_stones;
        init_stones[0] = dcs::ISimulator::StoneState(dc::Vector2(0.1f, 2.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        init_stones[15] = dcs::ISimulator::StoneState(dc::Vector2(0.1f, -4.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        simulator->SetStones(init_stones);
    }

    // 1
    auto storage = simulator->CreateStorage();  // create & save
    for (int i = 0; i < 10; ++i) { simulator->Step(); }
    auto const stones1 = simulator->GetStones();

    // 2
    auto simulator_copy = storage->CreateSimulator();
    for (int i = 0; i < 10; ++i) { simulator_copy->Step(); }
    auto const stones2 = simulator_copy->GetStones();

    EXPECT_TRUE(dct::EqualsSimulatorStones(stones1, stones2));
}

TEST(SimulatorFCV1, SaveLoad4)
{
    dcs::SimulatorFCV1Factory factory;
    auto simulator = factory.CreateSimulator();

    {
        dcs::ISimulator::AllStones init_stones;
        init_stones[0] = dcs::ISimulator::StoneState(dc::Vector2(0.1f, 2.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        init_stones[15] = dcs::ISimulator::StoneState(dc::Vector2(0.1f, -4.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        simulator->SetStones(init_stones);
    }

    // 1
    std::shared_ptr<dcs::ISimulatorStorage> storage = simulator->CreateStorage();
    json j_storage = *std::dynamic_pointer_cast<dcs::SimulatorFCV1Storage>(storage);
    for (int i = 0; i < 10; ++i) { simulator->Step(); }
    auto const stones1 = simulator->GetStones();

    // 2
    auto storage2 = j_storage.get<dcs::SimulatorFCV1Storage>();
    auto simulator_copy = storage2.CreateSimulator();
    for (int i = 0; i < 10; ++i) { simulator_copy->Step(); }
    auto const stones2 = simulator_copy->GetStones();

    EXPECT_TRUE(dct::EqualsSimulatorStones(stones1, stones2));
}


TEST(SimulatorFCV1, FactoryToJson)
{
    auto v_fcv1 = std::make_unique<dcs::SimulatorFCV1Factory>();
    v_fcv1->seconds_per_frame = 0.24f;

    json const j_fcv1 = *v_fcv1.get();
    EXPECT_EQ(j_fcv1.at("type").get<std::string>(), "fcv1");
    EXPECT_EQ(j_fcv1.at("seconds_per_frame").get<float>(), v_fcv1->seconds_per_frame);
}

TEST(SimulatorFCV1, FactoryFromJson)
{
    json const j_fcv1 = {
        { "type", "fcv1" },
        { "seconds_per_frame", 0.25f }
    };

    dcs::SimulatorFCV1Factory v_fcv1;
    EXPECT_NO_THROW(v_fcv1 = j_fcv1.get<dcs::SimulatorFCV1Factory>());
    EXPECT_EQ(v_fcv1.seconds_per_frame, 0.25f);
}
