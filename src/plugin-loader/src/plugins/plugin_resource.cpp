// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <memory>
#include <stdexcept>
#include "digitalcurling/plugins/native_loader.hpp"
#include "digitalcurling/plugins/plugin_resource.hpp"

#define DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(func_ptr) \
    do { \
        if (!(func_ptr)) { \
            throw std::runtime_error("PluginResource: Essential function '" #func_ptr "' is missing from the plugin API."); \
        } \
    } while (0)

namespace digitalcurling::plugins {

PluginResource::PluginResource(PluginInfo info, PluginApi api, ModulePtr handle)
    : info_(info),
      api_(api),
      handle_(handle),
      instance_list_(),
      create_factory(api.create_factory, api.free_string, api.destroy_factory, instance_list_),
      create_storage(api.create_storage, api.free_string, api.destroy_storage, instance_list_),
      create_target(api.create_target, api.free_string, api.destroy_target, instance_list_),
      object_creator_get_state(api.object_creator_get_state, api.free_string, instance_list_),
      factory_set_state(api.factory_set_state, api.free_string, instance_list_),
      storage_set_state(api.storage_set_state, api.free_string, instance_list_)
{
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(api.free_string);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(api.destroy_factory);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(create_factory);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(create_storage);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(create_target);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(object_creator_get_state);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(factory_set_state);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(storage_set_state);
}
PluginResource::~PluginResource()
{
    instance_list_.Clear();
    if (handle_) CloseSharedLibrary(handle_);
}
std::shared_ptr<PluginResource> PluginResource::Create(PluginInfo info, PluginApi api, ModulePtr handle) {
    if (info.plugin_type == DIGITALCURLING_PLUGIN_TYPE_SIMULATOR) {
        if (api.simulator) return std::make_shared<SimulatorPluginResource>(info, api, handle);
        throw std::runtime_error("PluginResource: Plugin type is SIMULATOR but SimulatorApi is missing.");
    } else if (info.plugin_type == DIGITALCURLING_PLUGIN_TYPE_PLAYER) {
        if (api.player) return std::make_shared<PlayerPluginResource>(info, api, handle);
        throw std::runtime_error("PluginResource: Plugin type is PLAYER but PlayerApi is missing.");
    } else {
        throw std::runtime_error("Plugin reported an unknown or invalid plugin type.");
    }
}

PlayerPluginResource::PlayerPluginResource(PluginInfo info, PluginApi api, ModulePtr handle)
    : PluginResource(info, api, handle),
      get_factory(api.player->get_factory, api.free_string, api.destroy_factory, instance_list_),
      save(api.player->save, api.free_string, instance_list_),
      load(api.player->load, api.free_string, instance_list_),
      get_gender(api.player->get_gender, api.free_string, instance_list_),
      play(api.player->play, api.free_string, instance_list_)
{
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(get_factory);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(save);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(load);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(get_gender);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(play);
}

SimulatorPluginResource::SimulatorPluginResource(PluginInfo info, PluginApi api, ModulePtr handle)
    : PluginResource(info, api, handle),
      get_factory(api.simulator->get_factory, api.free_string, api.destroy_factory, instance_list_),
      save(api.simulator->save, api.free_string, instance_list_),
      load(api.simulator->load, api.free_string, instance_list_),
      step(api.simulator->step, api.free_string, instance_list_),
      simulate(api.simulator->simulate, api.free_string, instance_list_),
      set_stones(api.simulator->set_stones, api.free_string, instance_list_),
      are_all_stones_stopped(api.simulator->are_all_stones_stopped, api.free_string, instance_list_),
      get_stones(api.simulator->get_stones, api.free_string, instance_list_),
      get_collisions(api.simulator->get_collisions, api.free_string, instance_list_),
      get_seconds_per_frame(api.simulator->get_seconds_per_frame, api.free_string, instance_list_),
      calculate_shot(api.simulator->calculate_shot, api.free_string, instance_list_)
{
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(get_factory);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(save);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(load);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(step);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(simulate);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(set_stones);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(are_all_stones_stopped);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(get_stones);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(get_collisions);
    DIGITALCURLING_PLUGIN_LOADER_CHECK_VALID_FUNC(get_seconds_per_frame);
}

} // namespace digitalcurling::plugins
