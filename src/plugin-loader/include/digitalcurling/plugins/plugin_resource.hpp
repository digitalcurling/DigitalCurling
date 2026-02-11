// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PluginResource とその派生クラスを定義

#pragma once

#include <memory>
#include <shared_mutex>
#include <string>
#include <uuidv7/uuidv7.hpp>
#include "digitalcurling/plugins/plugin_api.hpp"

#include "digitalcurling/plugins/native_loader.hpp"
#include "digitalcurling/plugins/plugin_class_type.hpp"
#include "digitalcurling/plugins/plugin_function.hpp"

namespace digitalcurling::plugins {

class PlayerPluginResource;
class SimulatorPluginResource;

class PluginResource {
public:
    PluginResource(const PluginResource&) = delete;
    PluginResource& operator=(const PluginResource&) = delete;
    PluginResource(PluginResource&&) = delete;
    PluginResource& operator=(PluginResource&&) = delete;

    virtual ~PluginResource();

    static std::shared_ptr<PluginResource> Create(PluginInfo info, PluginApi api, ModulePtr handle = nullptr);

    PluginType GetType() const { return static_cast<PluginType>(info_.plugin_type); }
    std::string GetName() const { return info_.plugin_name; }
    PluginInstanceList& GetInstanceList() { return instance_list_; }

    const PluginFunction<CreateFactoryFunc, uuidv7::uuidv7> create_factory;
    const PluginFunction<CreateStorageFunc, uuidv7::uuidv7> create_storage;
    const PluginFunction<CreateTargetFunc, uuidv7::uuidv7> create_target;
    const PluginFunction<ObjectCreatorGetStateFunc, std::string> object_creator_get_state;
    const PluginFunction<FactorySetStateFunc, void> factory_set_state;
    const PluginFunction<StorageSetStateFunc, void> storage_set_state;

protected:
    PluginResource(PluginInfo info, PluginApi api, ModulePtr handle);

    PluginInfo info_;
    PluginApi api_;
    ModulePtr handle_;
    PluginInstanceList instance_list_;
};

class PlayerPluginResource final : public PluginResource {
public:
    const PluginFunction<PlayerGetFactoryFunc, uuidv7::uuidv7> get_factory;
    const PluginFunction<PlayerSaveFunc, void> save;
    const PluginFunction<PlayerLoadFunc, void> load;

    const PluginFunction<PlayerGetGenderFunc, players::Gender> get_gender;
    const PluginFunction<PlayerPlayFunc, moves::Shot> play;

    explicit PlayerPluginResource(PluginInfo info, PluginApi api, ModulePtr handle);
};


class SimulatorPluginResource final : public PluginResource {
public:
    const PluginFunction<SimulatorGetFactoryFunc, uuidv7::uuidv7> get_factory;
    const PluginFunction<SimulatorSaveFunc, void> save;
    const PluginFunction<SimulatorLoadFunc, void> load;

    const PluginFunction<SimulatorStepFunc, void> step;
    const PluginFunction<SimulatorSimulateFunc, void> simulate;
    const PluginFunction<SimulatorSetStonesFunc, void> set_stones;

    const PluginFunction<SimulatorAreAllStonesStoppedFunc, bool> are_all_stones_stopped;
    const PluginFunction<SimulatorGetStonesFunc, simulators::ISimulator::AllStones> get_stones;
    const PluginFunction<SimulatorGetCollisionsFunc, std::string> get_collisions;
    const PluginFunction<SimulatorGetSecondsPerFrameFunc, float> get_seconds_per_frame;

    const PluginFunction<SimulatorCalculateShotFunc, moves::Shot> calculate_shot;

    explicit SimulatorPluginResource(PluginInfo info, PluginApi api, ModulePtr handle);

    bool IsInvertibleSimulator() const { return static_cast<bool>(calculate_shot); }
};

} // namespace digitalcurling::plugins
