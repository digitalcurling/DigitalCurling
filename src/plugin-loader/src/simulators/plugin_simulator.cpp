// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "digitalcurling/common.hpp"
#include "digitalcurling/simulators/plugin_simulator.hpp"
#include "digitalcurling/simulators/plugin_simulator_factory.hpp"
#include "digitalcurling/simulators/plugin_simulator_storage.hpp"

namespace digitalcurling::simulators {

PluginSimulator::PluginSimulator(std::string simulator_id, uuidv7::uuidv7 instance_id, std::weak_ptr<plugins::SimulatorPluginResource> owner_resource)
    : WrapperBase(std::move(simulator_id), std::move(instance_id), std::move(owner_resource))
      { }

void PluginSimulator::SetStones(ISimulator::AllStones const& stones) {
    ClearCaches();
    this->template ExecuteResourceFunc<void>([&](auto resource) {
        resource->set_stones.Execute(this->GetInstanceId(), stones);
    });
}
void PluginSimulator::Step(int frames, float sheet_width) {
    ClearCaches();
    this->template ExecuteResourceFunc<void>([&](auto resource) {
        resource->step.Execute(this->GetInstanceId(), frames, sheet_width);
    });
}
void PluginSimulator::Simulate(SimulateModeFlag mode_flag, float sheet_width) {
    ClearCaches();
    this->template ExecuteResourceFunc<void>([&](auto resource) {
        resource->simulate.Execute(this->GetInstanceId(), mode_flag, sheet_width);
    });
}

ISimulator::AllStones const& PluginSimulator::GetStones() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (all_stones_cache_.has_value()) return all_stones_cache_.value();

    this->template ExecuteResourceFunc<void>([&](auto resource) {
        all_stones_cache_ = resource->get_stones.Execute(this->GetInstanceId());
    });
    return all_stones_cache_.value();
}
std::vector<ISimulator::Collision> const& PluginSimulator::GetCollisions() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (collisions_cache_.has_value()) return collisions_cache_.value();

    this->template ExecuteResourceFunc<void>([&](auto resource) {
        collisions_cache_ = nlohmann::json::parse(resource->get_collisions.Execute(this->GetInstanceId()))
            .template get<std::vector<ISimulator::Collision>>();
    });
    return collisions_cache_.value();
}
bool PluginSimulator::AreAllStonesStopped() const {
    return this->template ExecuteResourceFunc<bool>([&](auto resource) {
        return resource->are_all_stones_stopped.Execute(this->GetInstanceId());
    });
}
float PluginSimulator::GetSecondsPerFrame() const {
    return this->template ExecuteResourceFunc<float>([&](auto resource) {
        return resource->get_seconds_per_frame.Execute(this->GetInstanceId());
    });
}

ISimulatorFactory const& PluginSimulator::GetFactory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (factory_cache_) return *factory_cache_;

    this->template ExecuteResourceFunc<void>([&](auto resource) {
        auto id = resource->get_factory.Execute(this->GetInstanceId());
        factory_cache_ = std::make_unique<PluginSimulatorFactory>(this->GetPluginId(), id, resource);
    });
    return *factory_cache_;
}
std::unique_ptr<ISimulatorStorage> PluginSimulator::CreateStorage() const {
    return this->template ExecuteResourceFunc<std::unique_ptr<ISimulatorStorage>>([&](auto resource) {
        auto id = resource->create_storage.Execute(nullptr);
        resource->save.Execute(this->GetInstanceId(), id);
        return std::make_unique<PluginSimulatorStorage>(this->GetPluginId(), id, resource);
    });
}
void PluginSimulator::Save(ISimulatorStorage& storage) const {
    auto plugin_storage = dynamic_cast<PluginSimulatorStorage*>(&storage);
    if (!plugin_storage || this->GetPluginId() != plugin_storage->GetSimulatorId())
        throw std::runtime_error("Failed to cast ISimulatorStorage to PluginSimulatorImplStorage for simulator: " + this->GetPluginId());

    this->template ExecuteResourceFunc<void>([&](auto resource) {
        resource->save.Execute(this->GetInstanceId(), plugin_storage->GetInstanceId());
    });
}
void PluginSimulator::Load(ISimulatorStorage const& storage) {
    auto plugin_storage = dynamic_cast<const PluginSimulatorStorage*>(&storage);
    if (!plugin_storage || this->GetPluginId() != plugin_storage->GetSimulatorId())
        throw std::runtime_error("Failed to cast ISimulatorStorage to PluginSimulatorImplStorage for simulator: " + this->GetPluginId());

    ClearCaches();
    this->template ExecuteResourceFunc<void>([&](auto resource) {
        resource->load.Execute(this->GetInstanceId(), plugin_storage->GetInstanceId());
    });
}

moves::Shot InvertiblePluginSimulator::CalculateShot(Vector2 const& target_position, float target_speed, float angular_velocity) const {
    return ExecuteResourceFunc<moves::Shot>([&](auto resource) {
        return resource->calculate_shot.Execute(GetInstanceId(), target_position, target_speed, angular_velocity);
    });
}

} // namespace digitalcurling::simulators
