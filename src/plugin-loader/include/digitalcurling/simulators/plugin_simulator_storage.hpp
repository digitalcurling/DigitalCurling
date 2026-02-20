// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PluginSimulatorStorage を定義

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <nlohmann/json.hpp>
#include <uuidv7/uuidv7.hpp>
#include "digitalcurling/simulators/i_simulator_storage.hpp"
#include "digitalcurling/simulators/plugin_simulator.hpp"

namespace digitalcurling::simulators {


/// @brief プラグインを経由したシミュレーターのストレージ
class PluginSimulatorStorage : public plugins::WrapperBase<digitalcurling::simulators::ISimulatorStorage> {
public:
    /// @brief コンストラクタ
    /// @param simulator_id シミュレーターID
    /// @param instance_id インスタンスID
    /// @param owner_resource 実際のインスタンスを持つリソースへの参照
    PluginSimulatorStorage(std::string simulator_id, uuidv7::uuidv7 instance_id, std::weak_ptr<OwnerResource> owner_resource)
        : WrapperBase(std::move(simulator_id), std::move(instance_id), std::move(owner_resource))
          {}

    virtual std::unique_ptr<ISimulator> CreateSimulator() const override {
        return ExecuteResourceFunc<std::unique_ptr<ISimulator>>([&](auto resource) -> std::unique_ptr<ISimulator> {
            auto id = resource->create_target.Execute(GetInstanceId());
            if (resource->IsInvertibleSimulator()) {
                return std::make_unique<InvertiblePluginSimulator>(GetPluginId(), id, resource);
            } else {
                return std::make_unique<PluginSimulator>(GetPluginId(), id, resource);
            }
        });
    }

    /// @brief このストレージの状態を JSON 形式で取得する
    /// @return JSON オブジェクト
    virtual nlohmann::json ToJson() const override {
        return ExecuteResourceFunc<nlohmann::json>([&](auto resource) {
            auto res = resource->object_creator_get_state.Execute(GetInstanceId());
            return nlohmann::json::parse(res);
        });
    }
};

} // namespace digitalcurling::simulators
