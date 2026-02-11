// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PluginManager を定義

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "digitalcurling/common.hpp"
#include "digitalcurling/plugins/plugin_api.hpp"
#include "digitalcurling/players/plugin_player_factory.hpp"
#include "digitalcurling/players/plugin_player_storage.hpp"
#include "digitalcurling/simulators/plugin_simulator_factory.hpp"
#include "digitalcurling/simulators/plugin_simulator_storage.hpp"

#include "digitalcurling_plugin_loader_export.h"
#include "digitalcurling/plugins/plugin_resource.hpp"

namespace digitalcurling::plugins {

/// @brief 共有ライブラリのファイル拡張子
extern const std::string LibraryExtension;

/// @brief プラグインの識別子
struct PluginId {
    /// @brief プラグインの種類
    PluginType type;
    /// @brief プラグイン名
    std::string name;
};

/// @brief プラグインを管理するクラス
class DIGITALCURLING_LOADER_API PluginManager {
public:
    /// @brief シングルトンインスタンスを取得する
    /// @return シングルトンインスタンスへの参照
    static PluginManager& GetInstance();

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) = delete;
    PluginManager& operator=(PluginManager&&) = delete;

#ifndef DIGITALCURLING_PLUGIN_LOADER_DISABLE_DYNAMIC
    /// @brief プラグインをロードする
    /// @param plugin_path ロードするプラグインのファイルパス
    /// @param throw_exception ロードに失敗した場合に例外を送出するか
    std::optional<PluginId> LoadPlugin(const std::filesystem::path& plugin_path, bool throw_exception);
#endif
    /// @brief プラグインリソースを登録する
    /// @param resource 登録するプラグインリソースへの共有ポインタ
    void RegisterPlugin(std::shared_ptr<PluginResource> resource);

    /// @brief ロード済みのプラグイン情報を取得する
    /// @param type プラグインの種類 (省略した場合はすべての種類)
    /// @return ロード済みのプラグインIDのリスト
    std::vector<PluginId> GetLoadedPlugins(std::optional<PluginType> type = std::nullopt) const;

    /// @brief プラグインがロードされているかを確認する
    /// @param type プラグインの種類
    /// @param name プラグイン名
    /// @return プラグインがロードされている場合は　`true`
    inline bool IsPluginLoaded(PluginType type, const std::string& name) const {
        std::shared_lock lock(mutex_);
        if (type == PluginType::player) {
            return player_resources_.count(name) > 0;
        } else if (type == PluginType::simulator) {
            return simulator_resources_.count(name) > 0;
        }
        return false;
    }

    // --- Player instance management ---

    /// @brief プレイヤーファクトリーを作成する
    /// @param name プラグイン名
    std::unique_ptr<players::PluginPlayerFactory> CreatePlayerFactory(const std::string& name);
    /// @brief プレイヤーファクトリーを作成する
    /// @param json ファクトリーの構築に使用するJSONオブジェクト
    std::unique_ptr<players::PluginPlayerFactory> CreatePlayerFactory(const nlohmann::json& json);
    /// @brief プレイヤーストレージを作成する
    /// @param name プラグイン名
    std::unique_ptr<players::PluginPlayerStorage> CreatePlayerStorage(const std::string& name);
    /// @brief プレイヤーストレージを作成する
    /// @param json ストレージの構築に使用するJSONオブジェクト
    std::unique_ptr<players::PluginPlayerStorage> CreatePlayerStorage(const nlohmann::json& json);

    // --- Simulator instance management ---

    /// @brief シミュレーターファクトリーを作成する
    /// @param name プラグイン名
    std::unique_ptr<simulators::PluginSimulatorFactory> CreateSimulatorFactory(const std::string& name);
    /// @brief シミュレーターファクトリーを作成する
    /// @param json ファクトリーの構築に使用するJSONオブジェクト
    std::unique_ptr<simulators::PluginSimulatorFactory> CreateSimulatorFactory(const nlohmann::json& json);
    /// @brief シミュレーターのストレージを作成する
    /// @param name プラグイン名
    std::unique_ptr<simulators::PluginSimulatorStorage> CreateSimulatorStorage(const std::string& name);
    /// @brief シミュレーターのストレージを作成する
    /// @param json ストレージの構築に使用するJSONオブジェクト
    std::unique_ptr<simulators::PluginSimulatorStorage> CreateSimulatorStorage(const nlohmann::json& json);

    /// @brief プレイヤープラグインのリソースを取得する
    /// @param name プラグイン名
    /// @return プレイヤープラグインのリソース (存在しない場合は `nullptr`)
    std::shared_ptr<PlayerPluginResource> GetPlayerPluginResource(const std::string& name) const {
        auto it = player_resources_.find(name);
        return it != player_resources_.end() ? it->second : nullptr;
    }
    /// @brief シミュレータープラグインのリソースを取得する
    /// @param name プラグイン名
    /// @return シミュレータープラグインのリソース (存在しない場合は `nullptr`)
    std::shared_ptr<SimulatorPluginResource> GetSimulatorPluginResource(const std::string& name) const {
        auto it = simulator_resources_.find(name);
        return it != simulator_resources_.end() ? it->second : nullptr;
    }

private:
    PluginManager() = default;
    ~PluginManager();

    mutable std::shared_mutex mutex_;

    std::unordered_map<std::string, std::shared_ptr<PlayerPluginResource>> player_resources_;
    std::unordered_map<std::string, std::shared_ptr<SimulatorPluginResource>> simulator_resources_;
};

} // namespace digitalcurling::plugins
