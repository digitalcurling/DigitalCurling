#include <cmath>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <uuidv7/uuidv7.hpp>
#include <uuidv7/generator.hpp>

#include "digitalcurling/common.hpp"
#include "digitalcurling/plugins/loader.h"
#include "digitalcurling/plugins/loader_types.h"
#include "digitalcurling/plugins/plugin_manager.hpp"
#include "digitalcurling/plugins/detail/c_type_converter.hpp"
#include "digitalcurling/players/plugin_player.hpp"
#include "digitalcurling/players/plugin_player_storage.hpp"
#include "digitalcurling/simulators/plugin_simulator.hpp"
#include "digitalcurling/simulators/plugin_simulator_storage.hpp"

#include "dynamic_test_base.hpp"

namespace {

using namespace digitalcurling;
using namespace digitalcurling::plugins;

class PluginLoaderDynamic : public digitalcurling::test::DynamicTestBase {
protected:
    std::optional<std::string> TryLoad(std::filesystem::path const& path, std::string const& expected_name) {
        if (std::filesystem::exists(path)) {
            std::string path_str = path.string();
            DigitalCurling_PluginType type;
            char name_buffer[256];
            DigitalCurling_ErrorCode err = dc_loader_load_plugin(path_str.c_str(), &type, name_buffer, sizeof(name_buffer));

            if (err != DIGITALCURLING_OK) return "error code " + std::to_string(err);
            else if (std::string(name_buffer) != expected_name) return "Loaded plugin name mismatch";
            return std::nullopt;
        }
        return "file not found";
    }
};

std::string GetSnapshotData(DigitalCurling_SnapshotHandle handle) {
    size_t required_size = 0;
    DigitalCurling_ErrorCode err = dc_loader_snapshot_get_data(handle, nullptr, 0, &required_size);
    if (err != DIGITALCURLING_OK || required_size == 0) {
        dc_loader_destroy_snapshot(handle);
        return "";
    }

    std::vector<char> buffer(required_size);
    err = dc_loader_snapshot_get_data(handle, buffer.data(), buffer.size(), &required_size);
    dc_loader_destroy_snapshot(handle);
    return err == DIGITALCURLING_OK ? std::string(buffer.data()) : "";
}

TEST_F(PluginLoaderDynamic, LoadAndUnload) {
    ASSERT_TRUE(IsSimPluginLoaded()) << "Simulator plugin failed to load in SetUp.";
    ASSERT_TRUE(IsPlayerPluginLoaded()) << "Player plugin failed to load in SetUp.";

    bool result;
    ASSERT_EQ(dc_loader_is_plugin_loaded(static_cast<DigitalCurling_PluginType>(PluginType::simulator), kSimPluginName, &result), DIGITALCURLING_OK);
    ASSERT_TRUE(result);
    ASSERT_EQ(dc_loader_is_plugin_loaded(static_cast<DigitalCurling_PluginType>(PluginType::player), kPlayerPluginName, &result), DIGITALCURLING_OK);
    ASSERT_TRUE(result);
}

TEST_F(PluginLoaderDynamic, LoadPluginErrors) {
    // 存在しないプラグイン
    DigitalCurling_PluginType type;
    char name_buffer[256];
    ASSERT_NE(dc_loader_load_plugin("non_existent.dll", &type, name_buffer, sizeof(name_buffer)), DIGITALCURLING_OK);

    // NULL 引数 (loader.cpp でチェックされている)
    ASSERT_EQ(dc_loader_load_plugin(nullptr, &type, name_buffer, sizeof(name_buffer)), DIGITALCURLING_ERR_INVALID_ARGUMENT);
    ASSERT_EQ(dc_loader_load_plugin(player_plugin_path_.c_str(), nullptr, name_buffer, sizeof(name_buffer)), DIGITALCURLING_ERR_BUFFER_NULLPTR);
    ASSERT_EQ(dc_loader_load_plugin(player_plugin_path_.c_str(), &type, nullptr, sizeof(name_buffer)), DIGITALCURLING_ERR_BUFFER_NULLPTR);

    if (IsPlayerPluginLoaded()) {
        DigitalCurling_PluginType type;
        char name_buffer[2]; // 明らかに小さいバッファ
        DigitalCurling_ErrorCode err = dc_loader_load_plugin(player_plugin_path_.c_str(), &type, name_buffer, sizeof(name_buffer));
        ASSERT_EQ(err, DIGITALCURLING_ERR_BUFFER_NULLPTR);
    }
}

TEST_F(PluginLoaderDynamic, InvalidInstanceErrors) {
    auto cpp_uuid = uuidv7::uuidv7_generator::generate_default();
    DigitalCurling_Uuid uuid;
    std::memcpy(&uuid, cpp_uuid.get_bytes().data(), 16);

    // 2. 存在しない Creator ID から Player/Simulator を作成しようとする
    DigitalCurling_Uuid out_id;
    ASSERT_EQ(dc_loader_create_player(&uuid, &out_id), DIGITALCURLING_ERR_INSTANCE_NOT_FOUND);
    ASSERT_EQ(dc_loader_create_simulator(&uuid, &out_id), DIGITALCURLING_ERR_INSTANCE_NOT_FOUND);

    // 3. 存在しない Player ID で Play を呼び出そうとする
    DigitalCurling_Shot input_shot = { 1.f, 0.f, 0.f };
    DigitalCurling_Shot output_shot;
    ASSERT_EQ(dc_loader_player_play(&uuid, &input_shot, &output_shot), DIGITALCURLING_ERR_INSTANCE_NOT_FOUND);

    // 4. 存在しないインスタンスを削除しようとする
    ASSERT_EQ(dc_loader_remove_player_instance(&uuid), DIGITALCURLING_ERR_INSTANCE_NOT_FOUND);
    ASSERT_EQ(dc_loader_remove_simulator_instance(&uuid), DIGITALCURLING_ERR_INSTANCE_NOT_FOUND);
}

TEST_F(PluginLoaderDynamic, UseAfterRemoveInstance) {
    if (!IsPlayerPluginLoaded()) {
        GTEST_SKIP() << "Player plugin not loaded, skipping test.";
    }

    DigitalCurling_Uuid factory_id;
    DigitalCurling_Uuid player_id;

    // 1. Factory と Player を作成
    ASSERT_EQ(dc_loader_create_player_factory(kPlayerPluginName, nullptr, &factory_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_player(&factory_id, &player_id), DIGITALCURLING_OK);

    // 2. Player を削除
    ASSERT_EQ(dc_loader_remove_player_instance(&player_id), DIGITALCURLING_OK);

    // 3. 削除された Player ID で Play を呼び出す (失敗)
    DigitalCurling_Shot input_shot = { 1.f, 0.f, 0.f };
    DigitalCurling_Shot output_shot;
    ASSERT_EQ(dc_loader_player_play(&player_id, &input_shot, &output_shot), DIGITALCURLING_ERR_INSTANCE_NOT_FOUND);

    // 4. Factory を削除
    ASSERT_EQ(dc_loader_remove_player_instance(&factory_id), DIGITALCURLING_OK);

    // 5. 削除された Factory ID で Player を作成しようとする (失敗)
    DigitalCurling_Uuid new_player_id;
    ASSERT_EQ(dc_loader_create_player(&factory_id, &new_player_id), DIGITALCURLING_ERR_INSTANCE_NOT_FOUND);
}


TEST_F(PluginLoaderDynamic, GetErrorMessage) {
    // 1. エラーを発生させる (NULL引数)
    ASSERT_NE(dc_loader_load_plugin(nullptr, nullptr, nullptr, 0), DIGITALCURLING_OK);

    // 2. エラーメッセージを取得 (サイズ計算)
    size_t required_size = 0;
    ASSERT_EQ(dc_loader_get_last_error_message(nullptr, 0, &required_size), DIGITALCURLING_OK);
    ASSERT_GT(required_size, 0);

    // 3. エラーメッセージを取得 (実データ)
    std::vector<char> buffer(required_size);
    ASSERT_EQ(dc_loader_get_last_error_message(buffer.data(), buffer.size(), &required_size), DIGITALCURLING_OK);

    // 4. 内容を確認 (関数名が含まれているはず)
    std::string message(buffer.data());
    ASSERT_NE(message.find("dc_loader_load_plugin"), std::string::npos);
}


TEST_F(PluginLoaderDynamic, Snapshot_LoadedPlugins) {
    if (!IsSimPluginLoaded() || !IsPlayerPluginLoaded()) {
        GTEST_SKIP() << "Both plugins must be loaded for this test.";
    }

    DigitalCurling_SnapshotHandle handle;
    size_t snapshot_size = 0;
    const char separator = ';';

    // 1. Playerプラグインのスナップショットを取得
    ASSERT_EQ(dc_loader_get_loaded_plugins_snapshot(DIGITALCURLING_PLUGIN_TYPE_PLAYER, separator, &handle, &snapshot_size), DIGITALCURLING_OK);
    std::string player_data = GetSnapshotData(handle);
    ASSERT_EQ(player_data.size() + 1, snapshot_size);
    ASSERT_EQ(player_data, std::string(kPlayerPluginName) + separator);

    // 2. Simulatorプラグインのスナップショットを取得
    ASSERT_EQ(dc_loader_get_loaded_plugins_snapshot(DIGITALCURLING_PLUGIN_TYPE_SIMULATOR, separator, &handle, &snapshot_size), DIGITALCURLING_OK);
    std::string sim_data = GetSnapshotData(handle);
    ASSERT_EQ(sim_data.size() + 1, snapshot_size);
    ASSERT_EQ(sim_data, std::string(kSimPluginName) + separator);
}

TEST_F(PluginLoaderDynamic, Snapshot_CreatorState) {
    if (!IsPlayerPluginLoaded()) {
        GTEST_SKIP() << "Player plugin not loaded, skipping test.";
    }

    DigitalCurling_Uuid factory_id;
    ASSERT_EQ(dc_loader_create_player_factory(kPlayerPluginName, nullptr, &factory_id), DIGITALCURLING_OK);

    DigitalCurling_SnapshotHandle handle;
    size_t snapshot_size = 0;
    ASSERT_EQ(dc_loader_creator_get_state_snapshot(&factory_id, &handle, &snapshot_size), DIGITALCURLING_OK);

    std::string json_data = GetSnapshotData(handle);
    ASSERT_FALSE(json_data.empty());
    ASSERT_EQ(json_data.size() + 1, snapshot_size);

    auto j = nlohmann::json::parse(json_data);
    ASSERT_TRUE(j.contains("gender"));

    ASSERT_EQ(dc_loader_remove_player_instance(&factory_id), DIGITALCURLING_OK);
}


TEST_F(PluginLoaderDynamic, Player_LifecycleAndAction) {
    if (!IsPlayerPluginLoaded()) {
        GTEST_SKIP() << "Player plugin not loaded, skipping test.";
    }

    DigitalCurling_Uuid factory_id;
    DigitalCurling_Uuid storage_id;
    DigitalCurling_Uuid player_id;

    // 1. Factory と Storage の作成
    ASSERT_EQ(dc_loader_create_player_factory(kPlayerPluginName, nullptr, &factory_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_player_storage(kPlayerPluginName, nullptr, &storage_id), DIGITALCURLING_OK);

    // 2. Player の作成 (Factoryから)
    ASSERT_EQ(dc_loader_create_player(&factory_id, &player_id), DIGITALCURLING_OK);

    // 3. Player のアクション (Play)
    DigitalCurling_Shot input_shot = { 1.f, 0.1f, 0.5f }; // speed, angle, rotation
    DigitalCurling_Shot output_shot;
    ASSERT_EQ(dc_loader_player_play(&player_id, &input_shot, &output_shot), DIGITALCURLING_OK);

    // "identical" プレイヤーなので、入力と出力が（ほぼ）一致するはず
    ASSERT_NEAR(input_shot.translational_velocity, output_shot.translational_velocity, 1e-6);
    ASSERT_NEAR(input_shot.angular_velocity, output_shot.angular_velocity, 1e-6);
    ASSERT_NEAR(input_shot.release_angle, output_shot.release_angle, 1e-6);

    // 4. インスタンスの削除 (loader.cpp の実装)
    ASSERT_EQ(dc_loader_remove_player_instance(&player_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_player_instance(&factory_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_player_instance(&storage_id), DIGITALCURLING_OK);

    // 5. 削除後のインスタンスで Play (失敗するはず)
    ASSERT_EQ(dc_loader_player_play(&player_id, &input_shot, &output_shot), DIGITALCURLING_ERR_INSTANCE_NOT_FOUND);
}

TEST_F(PluginLoaderDynamic, Player_SaveLoad) {
    if (!IsPlayerPluginLoaded()) {
        GTEST_SKIP() << "Player plugin not loaded, skipping test.";
    }

    DigitalCurling_Uuid factory_id, storage_id, player_id, player2_id;

    // 1. Factory, Storage, Player を作成
    // (PlayerFactory は "gender" を設定できると仮定)
    const char* json_config = "{\"gender\":\"male\"}";
    ASSERT_EQ(dc_loader_create_player_factory(kPlayerPluginName, json_config, &factory_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_player_storage(kPlayerPluginName, nullptr, &storage_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_player(&factory_id, &player_id), DIGITALCURLING_OK);

    // 2. Player の状態(Gender)を確認
    DigitalCurling_Gender gender;
    ASSERT_EQ(dc_loader_player_get_gender(&player_id, &gender), DIGITALCURLING_OK);
    ASSERT_EQ(gender, DIGITALCURLING_GENDER_MALE);

    // 3. Player -> Storage に Save
    ASSERT_EQ(dc_loader_player_save(&player_id, &storage_id), DIGITALCURLING_OK);

    // 4. 別の Player を作成 (初期状態は Female)
    const char* json_config2 = "{\"gender\":\"female\"}";
    DigitalCurling_Uuid factory2_id;
    ASSERT_EQ(dc_loader_create_player_factory(kPlayerPluginName, json_config2, &factory2_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_player(&factory2_id, &player2_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_player_get_gender(&player2_id, &gender), DIGITALCURLING_OK);
    ASSERT_EQ(gender, DIGITALCURLING_GENDER_FEMALE);

    // 5. Storage -> Player2 に Load
    ASSERT_EQ(dc_loader_player_load(&player2_id, &storage_id), DIGITALCURLING_OK);

    // 6. Player2 の状態が上書きされたか確認
    ASSERT_EQ(dc_loader_player_get_gender(&player2_id, &gender), DIGITALCURLING_OK);
    ASSERT_EQ(gender, DIGITALCURLING_GENDER_MALE);

    // 7. クリーンアップ
    ASSERT_EQ(dc_loader_remove_player_instance(&player_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_player_instance(&player2_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_player_instance(&factory_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_player_instance(&factory2_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_player_instance(&storage_id), DIGITALCURLING_OK);
}


TEST_F(PluginLoaderDynamic, Simulator_LifecycleAndActions) {
    if (!IsSimPluginLoaded()) {
        GTEST_SKIP() << "Simulator plugin not loaded, skipping test.";
    }

    DigitalCurling_Uuid factory_id;
    DigitalCurling_Uuid simulator_id;

    // 1. Factory と Simulator を作成
    const char* json_config = "{\"type\":\"fcv1\", \"seconds_per_frame\":0.01}";
    ASSERT_EQ(dc_loader_create_simulator_factory(kSimPluginName, json_config, &factory_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_simulator(&factory_id, &simulator_id), DIGITALCURLING_OK);

    // 2. 初期状態のストーンを取得
    DigitalCurling_StoneCoordinate stones_initial;
    ASSERT_EQ(dc_loader_simulator_get_stones(&simulator_id, &stones_initial), DIGITALCURLING_OK);
    ASSERT_FLOAT_EQ(stones_initial.stones[0].position.x, 0.f);
    ASSERT_FLOAT_EQ(stones_initial.stones[0].position.y, 0.f);
    ASSERT_FLOAT_EQ(stones_initial.stones[0].translational_velocity.y, 0.f);

    // 3. ストーンを設定 (C++側でDigitalCurling_StoneCoordinate構造体を準備)
    DigitalCurling_StoneCoordinate stones_to_set = {}; // ゼロ初期化
    stones_to_set.stones[0] = {
        {0.f, 10.f}, // position
        0.f, // angle
        {0.f, -1.f}, // translational_velocity
        0.f // angular_velocity
    };
    ASSERT_EQ(dc_loader_simulator_set_stones(&simulator_id, &stones_to_set), DIGITALCURLING_OK);

    // 4. 設定したストーンを取得 (Step前)
    DigitalCurling_StoneCoordinate stones_before_step;
    ASSERT_EQ(dc_loader_simulator_get_stones(&simulator_id, &stones_before_step), DIGITALCURLING_OK);
    ASSERT_NEAR(stones_before_step.stones[0].position.y, 10.f, 1e-6);
    ASSERT_NEAR(stones_before_step.stones[0].translational_velocity.y, -1.f, 1e-6);

    // 5. シミュレータを1フレーム進める
    bool is_stopped_before;
    ASSERT_EQ(dc_loader_simulator_are_all_stones_stopped(&simulator_id, &is_stopped_before), DIGITALCURLING_OK);
    ASSERT_FALSE(is_stopped_before); // 動いているはず

    ASSERT_EQ(dc_loader_simulator_step(&simulator_id, 1, 0), DIGITALCURLING_OK);

    // 6. Step後のストーンを取得
    DigitalCurling_StoneCoordinate stones_after_step;
    ASSERT_EQ(dc_loader_simulator_get_stones(&simulator_id, &stones_after_step), DIGITALCURLING_OK);

    // 7. 状態が変化したか確認
    float dt;
    ASSERT_EQ(dc_loader_simulator_get_seconds_per_frame(&simulator_id, &dt), DIGITALCURLING_OK);
    ASSERT_GT(dt, 0.f);
    ASSERT_NE(stones_after_step.stones[0].position.y, 10.f);
    ASSERT_NE(stones_after_step.stones[0].translational_velocity.y, -1.f);

    // 8. インスタンスの削除
    ASSERT_EQ(dc_loader_remove_simulator_instance(&simulator_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_simulator_instance(&factory_id), DIGITALCURLING_OK);
}

TEST_F(PluginLoaderDynamic, Simulator_SaveLoad) {
    if (!IsSimPluginLoaded()) {
        GTEST_SKIP() << "Simulator plugin not loaded, skipping test.";
    }

    DigitalCurling_Uuid factory_id, storage_id, sim_id, sim2_id;
    ASSERT_EQ(dc_loader_create_simulator_factory(kSimPluginName, nullptr, &factory_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_simulator_storage(kSimPluginName, nullptr, &storage_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_simulator(&factory_id, &sim_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_simulator(&factory_id, &sim2_id), DIGITALCURLING_OK);

    // 1. sim_id にストーンを設定
    DigitalCurling_StoneCoordinate stones_to_set = {};
    stones_to_set.stones[0] = { {0.f, 10.f}, 0.f, {0.f, -1.f}, 0.f };
    ASSERT_EQ(dc_loader_simulator_set_stones(&sim_id, &stones_to_set), DIGITALCURLING_OK);

    // 2. sim_id -> storage に Save
    ASSERT_EQ(dc_loader_simulator_save(&sim_id, &storage_id), DIGITALCURLING_OK);

    // 3. storage -> sim2_id に Load
    ASSERT_EQ(dc_loader_simulator_load(&sim2_id, &storage_id), DIGITALCURLING_OK);

    // 4. sim2_id のストーンが sim_id と同じか確認
    DigitalCurling_StoneCoordinate stones_loaded;
    ASSERT_EQ(dc_loader_simulator_get_stones(&sim2_id, &stones_loaded), DIGITALCURLING_OK);
    ASSERT_NEAR(stones_loaded.stones[0].position.y, 10.f, 1e-6);
    ASSERT_NEAR(stones_loaded.stones[0].translational_velocity.y, -1.f, 1e-6);

    // 5. クリーンアップ
    ASSERT_EQ(dc_loader_remove_simulator_instance(&sim_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_simulator_instance(&sim2_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_simulator_instance(&factory_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_simulator_instance(&storage_id), DIGITALCURLING_OK);
}

TEST_F(PluginLoaderDynamic, Simulator_SimulateAndCollisions) {
    if (!IsSimPluginLoaded()) {
        GTEST_SKIP() << "Simulator plugin not loaded, skipping test.";
    }

    DigitalCurling_Uuid factory_id, simulator_id;
    ASSERT_EQ(dc_loader_create_simulator_factory(kSimPluginName, nullptr, &factory_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_create_simulator(&factory_id, &simulator_id), DIGITALCURLING_OK);

    // 1. ショットの逆算
    DigitalCurling_Shot shot;
    DigitalCurling_Vector2 tee = { coordinate::kTee.x, coordinate::kTee.y };
    ASSERT_EQ(dc_loader_simulator_calculate_shot(&simulator_id, &tee, 0.2f, 1.57f, &shot), DIGITALCURLING_OK);
    float shot_vector_x = shot.translational_velocity * std::cos(shot.release_angle);
    float shot_vector_y = shot.translational_velocity * std::sin(shot.release_angle);

    // 2. 1投目のストーンを設定 (Simulate Full)
    DigitalCurling_StoneCoordinate coordinate;
    coordinate.stones[0] = { {0.f, 0.f}, 0.f, {shot_vector_x, shot_vector_y}, 1.57f };
    ASSERT_EQ(dc_loader_simulator_set_stones(&simulator_id, &coordinate), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_simulator_simulate(&simulator_id, DIGITALCURLING_SIMULATE_MODE_FULL, 4.75f), DIGITALCURLING_OK);
    bool all_stopped;
    ASSERT_EQ(dc_loader_simulator_are_all_stones_stopped(&simulator_id, &all_stopped), DIGITALCURLING_OK);
    ASSERT_TRUE(all_stopped);

    // 3. 2投目のストーンを設定 (Simulate Collision)
    ASSERT_EQ(dc_loader_simulator_get_stones(&simulator_id, &coordinate), DIGITALCURLING_OK);
    coordinate.stones[1] = { {0.f, 0.f}, 0.f, {shot_vector_x, shot_vector_y}, 1.57f };
    ASSERT_EQ(dc_loader_simulator_set_stones(&simulator_id, &coordinate), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_simulator_simulate(&simulator_id, DIGITALCURLING_SIMULATE_MODE_COLLISION, 0.f), DIGITALCURLING_OK);

    // 4. 衝突スナップショットを取得
    DigitalCurling_SnapshotHandle handle;
    size_t snapshot_size = 0;
    ASSERT_EQ(dc_loader_simulator_get_collisions_snapshot(&simulator_id, &handle, &snapshot_size), DIGITALCURLING_OK);
    std::string json_data = GetSnapshotData(handle);
    ASSERT_FALSE(json_data.empty());
    ASSERT_EQ(json_data.size() + 1, snapshot_size);

    // 5. 衝突が発生したか検証 (fcv1は衝突情報をjsonで返す)
    auto j = nlohmann::json::parse(json_data);
    ASSERT_TRUE(j.is_array());
    ASSERT_FALSE(j.empty());

    // 6. クリーンアップ
    ASSERT_EQ(dc_loader_remove_simulator_instance(&simulator_id), DIGITALCURLING_OK);
    ASSERT_EQ(dc_loader_remove_simulator_instance(&factory_id), DIGITALCURLING_OK);
}

} // namespace
