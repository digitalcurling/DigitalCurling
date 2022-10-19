#include <iostream>
#include "common.hpp"

namespace dc = digitalcurling3;
using nlohmann::json;

namespace {

bool EqualsSimulatorStones(dc::ISimulator::AllStones const& stones0, dc::ISimulator::AllStones const& stones1)
{
    bool equal = true;
    for (size_t i = 0; i < stones0.size(); ++i) {
        if (stones0[i].has_value() != stones1[i].has_value()) {
            equal = false;
            break;
        }
        if (stones0[i].has_value()) {
            if (stones0[i]->position.x != stones1[i]->position.x ||
                stones0[i]->position.y != stones1[i]->position.y ||
                stones0[i]->angle != stones1[i]->angle ||
                stones0[i]->linear_velocity.x != stones1[i]->linear_velocity.x ||
                stones0[i]->linear_velocity.y != stones1[i]->linear_velocity.y ||
                stones0[i]->angular_velocity != stones1[i]->angular_velocity) {
                equal = false;
                break;
            }
        }
    }

    return equal;
}

void PrintSimulatorStones(dc::ISimulator::AllStones const& stones)
{
    for (size_t i = 0; i < stones.size(); ++i) {
        std::cout << i << ": ";
        if (stones[i].has_value()) {
            std::cout << "pos.x: " << stones[i]->position.x << ", "
                << "pos.y: " << stones[i]->position.y << ", "
                << "angle: " << stones[i]->angle << ", "
                << "lv.x: " << stones[i]->linear_velocity.x << ", "
                << "lv.y: " << stones[i]->linear_velocity.y << ", "
                << "av: " << stones[i]->angular_velocity;
        } else {
            std::cout << "none";
        }

        std::cout << std::endl;
    }
}

} // unnamed namespace

TEST(FCV1Simulator, InitialState)
{
    dc::simulators::FCV1SimulatorFactory factory;
    auto simulator = factory.CreateSimulator();
    EXPECT_TRUE(simulator->AreAllStonesStopped());
}

TEST(FCV1Simulator, SaveLoad)
{
    dc::simulators::FCV1SimulatorFactory factory;
    auto simulator = factory.CreateSimulator();

    {
        dc::ISimulator::AllStones init_stones;
        init_stones[0] = dc::ISimulator::Stone(dc::Vector2(0.1f, 2.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        init_stones[15] = dc::ISimulator::Stone(dc::Vector2(0.1f, -4.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
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
    
    EXPECT_TRUE(EqualsSimulatorStones(stones1, stones2));
}

TEST(FCV1Simulator, SaveLoad2)
{
    dc::simulators::FCV1SimulatorFactory factory;
    auto simulator = factory.CreateSimulator();
    auto storage = simulator->CreateStorage();  // create & save

    {
        dc::ISimulator::AllStones init_stones;
        init_stones[0] = dc::ISimulator::Stone(dc::Vector2(0.1f, 2.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        init_stones[15] = dc::ISimulator::Stone(dc::Vector2(0.1f, -4.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
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
    
    EXPECT_TRUE(EqualsSimulatorStones(stones1, stones2));
}

TEST(FCV1Simulator, SaveLoad3)
{
    dc::simulators::FCV1SimulatorFactory factory;
    auto simulator = factory.CreateSimulator();

    {
        dc::ISimulator::AllStones init_stones;
        init_stones[0] = dc::ISimulator::Stone(dc::Vector2(0.1f, 2.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        init_stones[15] = dc::ISimulator::Stone(dc::Vector2(0.1f, -4.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
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
    
    EXPECT_TRUE(EqualsSimulatorStones(stones1, stones2));
}

TEST(FCV1Simulator, SaveLoad4)
{
    dc::simulators::FCV1SimulatorFactory factory;
    auto simulator = factory.CreateSimulator();

    {
        dc::ISimulator::AllStones init_stones;
        init_stones[0] = dc::ISimulator::Stone(dc::Vector2(0.1f, 2.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        init_stones[15] = dc::ISimulator::Stone(dc::Vector2(0.1f, -4.f), 0.5f, dc::Vector2(-0.1f, -2.f), 1.f);
        simulator->SetStones(init_stones);
    }

    // 1
    auto storage = simulator->CreateStorage();  // create & save
    json j_storage = *storage;
    for (int i = 0; i < 10; ++i) { simulator->Step(); }
    auto const stones1 = simulator->GetStones();

    // 2
    auto storage2 = j_storage.get<std::unique_ptr<dc::ISimulatorStorage>>();
    auto simulator_copy = storage2->CreateSimulator();
    for (int i = 0; i < 10; ++i) { simulator_copy->Step(); }
    auto const stones2 = simulator_copy->GetStones();
    
    EXPECT_TRUE(EqualsSimulatorStones(stones1, stones2));
}
