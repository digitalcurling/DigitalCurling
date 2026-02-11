#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "digitalcurling/common.hpp"
#include "digitalcurling/plugins/plugin_manager.hpp"
#include "digitalcurling/players/plugin_player_factory.hpp"
#include "digitalcurling/players/plugin_player_storage.hpp"
#include "digitalcurling/players/plugin_player.hpp"
#include "digitalcurling/simulators/plugin_simulator_factory.hpp"
#include "digitalcurling/simulators/plugin_simulator_storage.hpp"
#include "digitalcurling/simulators/plugin_simulator.hpp"

#include "dynamic_test_base.hpp"

namespace {

using namespace digitalcurling;
using namespace digitalcurling::plugins;

class PluginManagerDynamic : public digitalcurling::test::DynamicTestBase {
protected:
    PluginManager* manager_ = nullptr;

    void SetUp() override {
        manager_ = &PluginManager::GetInstance();
        digitalcurling::test::DynamicTestBase::SetUp();
    }

    std::optional<std::string> TryLoad(std::filesystem::path const& path, std::string const& expected_name) override {
        if (std::filesystem::exists(path)) {
            try {
                auto loaded = manager_->LoadPlugin(path, true);
                if (!loaded.has_value() || loaded->name != expected_name)
                    return "Loaded plugin name mismatch";
                return std::nullopt;
            } catch (const std::exception& e) {
                return e.what();
            }
        }
        return "file not found";
    }
};


TEST_F(PluginManagerDynamic, LoadPlugins) {
    ASSERT_TRUE(IsSimPluginLoaded()) << "Simulator plugin failed to load in SetUp.";
    ASSERT_TRUE(IsPlayerPluginLoaded()) << "Player plugin failed to load in SetUp.";

    EXPECT_TRUE(manager_->IsPluginLoaded(PluginType::simulator, kSimPluginName));
    EXPECT_TRUE(manager_->IsPluginLoaded(PluginType::player, kPlayerPluginName));

    auto sim_plugins = manager_->GetLoadedPlugins(PluginType::simulator);
    auto player_plugins = manager_->GetLoadedPlugins(PluginType::player);

    EXPECT_EQ(sim_plugins.size(), 1);
    EXPECT_EQ(sim_plugins[0].name, std::string(kSimPluginName));
    EXPECT_EQ(player_plugins.size(), 1);
    EXPECT_EQ(player_plugins[0].name, std::string(kPlayerPluginName));
}

TEST_F(PluginManagerDynamic, LoadNonExistentPlugin) {
    std::filesystem::path bad_path = "non_existent_plugin" + LibraryExtension;
    ASSERT_THROW(manager_->LoadPlugin(bad_path, true), std::runtime_error);
}

TEST_F(PluginManagerDynamic, LoadNonExistentPlugin_ExceptionFalse) {
    std::filesystem::path bad_path = "non_existent_plugin" + LibraryExtension;
    std::optional<PluginId> result;
    ASSERT_NO_THROW(result = manager_->LoadPlugin(bad_path, false));
    ASSERT_FALSE(result.has_value());
}

TEST_F(PluginManagerDynamic, Player_Lifecycle) {
    if (!IsPlayerPluginLoaded()) {
        GTEST_SKIP() << "Player plugin not loaded, skipping test.";
    }

    std::unique_ptr<players::IPlayer> player_target;
    {
        std::unique_ptr<players::PluginPlayerFactory> player_factory;
        ASSERT_NO_THROW(player_factory = manager_->CreatePlayerFactory(std::string(kPlayerPluginName)));
        ASSERT_NE(player_factory, nullptr);

        ASSERT_NO_THROW(player_target = player_factory->CreatePlayer());
        ASSERT_NE(player_target, nullptr);
        ASSERT_EQ(player_target->GetPlayerId(), std::string(kPlayerPluginName));
    }

    ASSERT_NE(player_target, nullptr);
}

TEST_F(PluginManagerDynamic, Player_SaveAndLoad) {
    if (!IsPlayerPluginLoaded()) {
        GTEST_SKIP() << "Player plugin not loaded, skipping test.";
    }

    auto factory = manager_->CreatePlayerFactory(std::string(kPlayerPluginName));

    // 1. 性別を設定できる Factory/Storage か確認
    factory->SetGender(players::Gender::kMale);
    ASSERT_EQ(factory->GetGender(), players::Gender::kMale);

    // 2. Player と Storage を作成
    std::unique_ptr<players::IPlayer> player = factory->CreatePlayer();
    std::unique_ptr<players::PluginPlayerStorage> storage = manager_->CreatePlayerStorage(std::string(kPlayerPluginName));

    // 3. Player の状態 (Gender) を確認
    ASSERT_EQ(player->GetGender(), players::Gender::kMale);

    // 4. Player -> Storage に Save
    ASSERT_NO_THROW(player->Save(*storage));

    // 5. Storage の状態 (Gender) を確認
    ASSERT_EQ(storage->GetGender(), players::Gender::kMale);

    // 6. 別の Player を作成
    auto factory2 = manager_->CreatePlayerFactory(std::string(kPlayerPluginName));
    factory2->SetGender(players::Gender::kFemale);
    std::unique_ptr<players::IPlayer> player2 = factory2->CreatePlayer();
    ASSERT_EQ(player2->GetGender(), players::Gender::kFemale);

    // 7. Storage -> Player2 に Load
    ASSERT_NO_THROW(player2->Load(*storage));

    // 8. Player2 の状態が Storage の状態 (Male) に上書きされたか確認
    EXPECT_EQ(player2->GetGender(), players::Gender::kMale);
}

TEST_F(PluginManagerDynamic, Player_GetFactory) {
    if (!IsPlayerPluginLoaded()) {
        GTEST_SKIP() << "Player plugin not loaded, skipping test.";
    }
    auto factory = manager_->CreatePlayerFactory(std::string(kPlayerPluginName));
    auto player = factory->CreatePlayer();

    const auto& factory_ref = player->GetFactory();
    ASSERT_EQ(factory_ref.GetPlayerId(), std::string(kPlayerPluginName));

    std::unique_ptr<players::IPlayerFactory> cloned_factory = nullptr;
    ASSERT_NO_THROW(cloned_factory = factory_ref.Clone());
    ASSERT_NE(cloned_factory, nullptr);
    ASSERT_EQ(cloned_factory->GetPlayerId(), std::string(kPlayerPluginName));
}


TEST_F(PluginManagerDynamic, Simulator_Lifecycle) {
    if (!IsSimPluginLoaded()) {
        GTEST_SKIP() << "Simulator plugin not loaded, skipping test.";
    }

    std::unique_ptr<simulators::PluginSimulatorFactory> sim_factory;
    {
        nlohmann::json sim_factory_json = { {"type", std::string(kSimPluginName)}, {"seconds_per_frame", 0.001} };
        ASSERT_NO_THROW(sim_factory = manager_->CreateSimulatorFactory(sim_factory_json));
        ASSERT_NE(sim_factory, nullptr);
        ASSERT_EQ(sim_factory->GetSimulatorId(), std::string(kSimPluginName));
    }

    std::unique_ptr<simulators::PluginSimulatorStorage> sim_storage;
    std::unique_ptr<simulators::ISimulator> sim_target;

    ASSERT_NO_THROW(sim_storage = manager_->CreateSimulatorStorage(std::string(kSimPluginName)));
    ASSERT_NE(sim_storage, nullptr);

    ASSERT_NO_THROW(sim_target = sim_storage->CreateSimulator());
    ASSERT_NE(sim_target, nullptr);
    ASSERT_EQ(sim_target->GetSimulatorId(), std::string(kSimPluginName));
}

TEST_F(PluginManagerDynamic, Simulator_GetFactory) {
    if (!IsSimPluginLoaded()) {
        GTEST_SKIP() << "Simulator plugin not loaded, skipping test.";
    }
    auto factory = manager_->CreateSimulatorFactory(std::string(kSimPluginName));
    auto simulator = factory->CreateSimulator();

    const auto& factory_ref = simulator->GetFactory();
    ASSERT_EQ(factory_ref.GetSimulatorId(), std::string(kSimPluginName));

    std::unique_ptr<simulators::ISimulatorFactory> cloned_factory = nullptr;
    ASSERT_NO_THROW(cloned_factory = factory_ref.Clone());
    ASSERT_NE(cloned_factory, nullptr);
    ASSERT_EQ(cloned_factory->GetSimulatorId(), std::string(kSimPluginName));
}

TEST_F(PluginManagerDynamic, Simulator_StepAndGetStones) {
    if (!IsSimPluginLoaded()) {
        GTEST_SKIP() << "Simulator plugin not loaded, skipping test.";
    }

    auto factory = manager_->CreateSimulatorFactory(std::string(kSimPluginName));
    auto simulator = factory->CreateSimulator();

    // 1. 初期状態のストーンを取得
    simulators::ISimulator::AllStones stones_initial;
    ASSERT_NO_THROW(stones_initial = simulator->GetStones());
    ASSERT_FALSE(stones_initial[0].has_value());

    // 2. ストーンを設定
    simulators::ISimulator::AllStones stones_to_set;
    stones_to_set[0] = simulators::ISimulator::StoneState{
        {0.f, 10.f}, // position
        0.f, // angle
        {0.f, -1.f}, // translational_velocity
        0.f // angular_velocity
    };
    ASSERT_NO_THROW(simulator->SetStones(stones_to_set));

    // 3. 設定したストーンを取得 (Step前)
    auto stones_before_step = simulator->GetStones();
    ASSERT_TRUE(stones_before_step[0].has_value());
    ASSERT_NEAR(stones_before_step[0]->position.y, 10.f, 1e-6);
    ASSERT_NEAR(stones_before_step[0]->translational_velocity.y, -1.f, 1e-6);

    // 4. シミュレータを1フレーム進める
    ASSERT_NO_THROW(simulator->Step());

    // 5. Step後のストーンを取得
    auto stones_after_step = simulator->GetStones();
    ASSERT_TRUE(stones_after_step[0].has_value());
    float dt = simulator->GetSecondsPerFrame();
    float expected_pos_y = 10.f + (-1.f * dt);
    ASSERT_NE(stones_after_step[0]->position.y, 10.f);
    ASSERT_NE(stones_after_step[0]->translational_velocity.y, -1.f);
}

TEST_F(PluginManagerDynamic, Simulator_SimulateAndGetCollisions) {
    if (!IsSimPluginLoaded()) {
        GTEST_SKIP() << "Simulator plugin not loaded, skipping test.";
    }

    auto factory = manager_->CreateSimulatorFactory(std::string(kSimPluginName));
    auto simulator = factory->CreateSimulator();
    auto* plugin_sim = dynamic_cast<simulators::InvertiblePluginSimulator*>(simulator.get());
    ASSERT_NE(plugin_sim, nullptr);

    // 1. ショットの逆算
    moves::Shot shot;
    ASSERT_NO_THROW(shot = plugin_sim->CalculateShot(coordinate::kTee, 0.2f, 1.57f));
    auto shot_stone = simulators::ISimulator::StoneState{
        {0.f, 0.f},
        0.f,
        shot.ToVector2(),
        shot.angular_velocity
    };

    // 2. 1投目のストーンを設定 (Simulate Full)
    simulators::ISimulator::AllStones stones;
    stones[0] = shot_stone;
    ASSERT_NO_THROW(plugin_sim->SetStones(stones));
    ASSERT_NO_THROW(plugin_sim->Simulate(simulators::SimulateModeFlag::Full, 4.75f));
    EXPECT_TRUE(plugin_sim->AreAllStonesStopped());

    // 3. 2投目のストーンを設定 (Simulate Collision)
    ASSERT_NO_THROW(stones = plugin_sim->GetStones());
    stones[1] = shot_stone;
    ASSERT_NO_THROW(plugin_sim->SetStones(stones));
    ASSERT_NO_THROW(plugin_sim->Simulate(simulators::SimulateModeFlag::Collision, 4.75f));

    // 4. GetCollisions
    std::vector<simulators::ISimulator::Collision> collisions;
    ASSERT_NO_THROW(collisions = simulator->GetCollisions());
    ASSERT_FALSE(collisions.empty());
}

} // namespace
