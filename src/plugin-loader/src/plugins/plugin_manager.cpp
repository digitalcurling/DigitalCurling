// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <nlohmann/json.hpp>
#include "digitalcurling/common.hpp"
#include "digitalcurling/plugins/detail/native_loader.hpp"
#include "digitalcurling/plugins/plugin_manager.hpp"

namespace digitalcurling::plugins {


#ifndef DIGITALCURLING_LIBRARY_EXTENSION
    #error "Build system must define DIGITALCURLING_LIBRARY_EXTENSION. Check CMakeLists.txt. for digitalcurling_plugin_loader."
#endif
const std::string LibraryExtension = DIGITALCURLING_LIBRARY_EXTENSION;

#ifdef DIGITALCURLING_BUNDLE_PLUGINS
    void RegisterStaticPlugin(PluginInfo info, PluginApi api) {
        auto resource = digitalcurling::plugins::detail::PluginResource::Create(info, api, std::nullopt);
        digitalcurling::plugins::PluginManager::GetInstance().RegisterPlugin(std::move(resource));
    }
#endif // DIGITALCURLING_BUNDLE_PLUGINS


PluginManager::~PluginManager() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    player_resources_.clear();
    simulator_resources_.clear();
}

PluginManager& PluginManager::GetInstance() {
    static PluginManager instance;
    return instance;
}

#ifndef DIGITALCURLING_PLUGIN_LOADER_DISABLE_DYNAMIC
std::optional<PluginId> PluginManager::LoadPlugin(const std::filesystem::path& plugin_path, bool throw_exception) {
    auto exit = [&](const std::string& msg) -> std::optional<PluginId> {
        if (!throw_exception) return std::nullopt;
        else throw plugin_error { DIGITALCURLING_ERR_FAILED_TO_LOAD_PLUGIN, "\"" + plugin_path.string() + "\": " + msg };
    };

    if (!std::filesystem::exists(plugin_path) || !std::filesystem::is_regular_file(plugin_path))
        return exit("File does not exist or is not a regular file.");
    if (!plugin_path.has_extension() || plugin_path.extension() != LibraryExtension)
        return exit("Invalid file extension. Expected " + LibraryExtension + ".");

    try {
        auto native = detail::LoadNativePlugin(plugin_path);

        auto info = native.get_plugin_info();
        if (info.plugin_version != DIGITALCURLING_PLUGIN_API_VERSION) {
            throw plugin_error{
                DIGITALCURLING_ERR_PLUGIN_VERSION_MISMATCH,
                "Plugin version is incompatible (expected version " + std::to_string(DIGITALCURLING_PLUGIN_API_VERSION) + ")."
            };
        }
        auto api = native.get_plugin_api();

        auto id = PluginId{ static_cast<PluginType>(info.plugin_type), info.plugin_name };
        auto resource = detail::PluginResource::Create(std::move(info), std::move(api), std::move(native.handle));
        RegisterPlugin(std::move(resource));
        return id;
    } catch (const std::exception& e) {
        if (!throw_exception) return std::nullopt;
        throw;
    }
}
#endif // DIGITALCURLING_PLUGIN_LOADER_DISABLE_DYNAMIC

void PluginManager::RegisterPlugin(std::shared_ptr<detail::PluginResource> resource) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto type = resource->GetType();
    if (type == PluginType::player) {
        auto player_resource = std::dynamic_pointer_cast<detail::PlayerPluginResource>(resource);
        if (player_resource) {
            player_resources_[resource->GetName()] = std::move(player_resource);
            return;
        }
    } else if (type == PluginType::simulator) {
        auto simulator_resource = std::dynamic_pointer_cast<detail::SimulatorPluginResource>(resource);
        if (simulator_resource) {
            simulator_resources_[resource->GetName()] = std::move(simulator_resource);
            return;
        }
    } else {
        throw plugin_error{DIGITALCURLING_ERR_INVALID_PLUGIN_TYPE, "The plugin is not supported."};
    }

    throw plugin_error {
        DIGITALCURLING_ERR_FAILED_TO_CAST,
        "Failed to load " + ToString(type) + " plugin: " + resource->GetName()
    };
}

std::vector<PluginId> PluginManager::GetLoadedPlugins(std::optional<PluginType> type) const {
    std::shared_lock lock(mutex_);
    std::vector<PluginId> ids;

    if (!type.has_value()) {
        ids.reserve(player_resources_.size() + simulator_resources_.size());
        for (const auto& pair : player_resources_) ids.push_back({PluginType::player, pair.first});
        for (const auto& pair : simulator_resources_) ids.push_back({PluginType::simulator, pair.first});
        return ids;
    } else if (type == PluginType::player) {
        for (const auto& pair : player_resources_) ids.push_back({PluginType::player, pair.first});
    } else if (type == PluginType::simulator) {
        for (const auto& pair : simulator_resources_) ids.push_back({PluginType::simulator, pair.first});
    } else {
        throw plugin_error{DIGITALCURLING_ERR_INVALID_PLUGIN_TYPE, "Plugin type is invalid."};
    }
    return ids;
}

// --- Player instance management ---
std::unique_ptr<players::PluginPlayerFactory> PluginManager::CreatePlayerFactory(const std::string& name) {
    std::shared_lock lock(mutex_);
    auto res = GetPlayerPluginResource(name);
    return std::make_unique<players::PluginPlayerFactory>(name, res->create_factory.Execute(nullptr), res);
}
std::unique_ptr<players::PluginPlayerFactory> PluginManager::CreatePlayerFactory(const nlohmann::json& json) {
    std::shared_lock lock(mutex_);
    auto name = json.at("type").get<std::string>();
    auto res = GetPlayerPluginResource(name);
    return std::make_unique<players::PluginPlayerFactory>(name, res->create_factory.Execute(json.dump().c_str()), res);
}
std::unique_ptr<players::PluginPlayerStorage> PluginManager::CreatePlayerStorage(const std::string& name) {
    std::shared_lock lock(mutex_);
    auto res = GetPlayerPluginResource(name);
    return std::make_unique<players::PluginPlayerStorage>(name, res->create_storage.Execute(nullptr), res);
}
std::unique_ptr<players::PluginPlayerStorage> PluginManager::CreatePlayerStorage(const nlohmann::json& json) {
    std::shared_lock lock(mutex_);
    auto name = json.at("type").get<std::string>();
    auto res = GetPlayerPluginResource(name);
    return std::make_unique<players::PluginPlayerStorage>(name, res->create_storage.Execute(json.dump().c_str()), res);
}

// --- Simulator instance management ---
std::unique_ptr<simulators::PluginSimulatorFactory> PluginManager::CreateSimulatorFactory(const std::string& name) {
    std::shared_lock lock(mutex_);
    auto res = GetSimulatorPluginResource(name);
    return std::make_unique<simulators::PluginSimulatorFactory>(name, res->create_factory.Execute(nullptr), res);
}
std::unique_ptr<simulators::PluginSimulatorFactory> PluginManager::CreateSimulatorFactory(const nlohmann::json& json) {
    std::shared_lock lock(mutex_);
    auto name = json.at("type").get<std::string>();
    auto res = GetSimulatorPluginResource(name);
    return std::make_unique<simulators::PluginSimulatorFactory>(name, res->create_factory.Execute(json.dump().c_str()), res);
}
std::unique_ptr<simulators::PluginSimulatorStorage> PluginManager::CreateSimulatorStorage(const std::string& name) {
    std::shared_lock lock(mutex_);
    auto res = GetSimulatorPluginResource(name);
    return std::make_unique<simulators::PluginSimulatorStorage>(name, res->create_storage.Execute(nullptr), res);
}
std::unique_ptr<simulators::PluginSimulatorStorage> PluginManager::CreateSimulatorStorage(const nlohmann::json& json) {
    std::shared_lock lock(mutex_);
    auto name = json.at("type").get<std::string>();
    auto res = GetSimulatorPluginResource(name);
    return std::make_unique<simulators::PluginSimulatorStorage>(name, res->create_storage.Execute(json.dump().c_str()), res);
}

} // namespace digitalcurling::plugins
