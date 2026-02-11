// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <nlohmann/json.hpp>
#include "digitalcurling/moves/shot.hpp"
#include "digitalcurling/plugins/plugin_manager.hpp"
#include "digitalcurling/plugins/plugin_type.hpp"
#include "digitalcurling/plugins/loader.h"

// --- Macros ---
#define DIGITALCURLING_LOADER_RETURN_ERROR(err_code, err_msg) \
    do { \
        last_error_message = std::string(__func__) + ": " + std::string(err_msg); \
        return err_code; \
    } while (0)

#define DIGITALCURLING_LOADER_CHECK_POINTER(arg) \
    do { \
        if ((arg) == nullptr) { \
            if constexpr (std::string_view{#arg}.compare(0, 4, "out_") == 0) \
            { \
                DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_BUFFER_NULLPTR, std::string(#arg) + " is nullptr."); \
            } else { \
                DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INVALID_ARGUMENT, std::string(#arg) + " is nullptr."); \
            } \
        } \
    } while (0)

#define DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result) \
    do { \
        if (!(result)) { \
            auto err = (result).GetError(); \
            last_error_message = std::string(__func__) + ": " + std::string(err.what()); \
            return static_cast<DigitalCurling_ErrorCode>(err.code()); \
        } \
    } while (0)


// --- Unnamed Namespace ---
using PluginManager = digitalcurling::plugins::PluginManager;
using PluginType = digitalcurling::plugins::PluginType;
using PluginId = digitalcurling::plugins::PluginId;

namespace {
    static thread_local std::string last_error_message;

    DigitalCurling_ErrorCode catch_exceptions(const char* func_name, std::function<DigitalCurling_ErrorCode()> func) {
        auto set_last_error = [&](const char* msg) {
            last_error_message = std::string(func_name) + ": " + std::string(msg);
        };

        try {
            return func();
        } catch (const nlohmann::json::exception& e) {
            set_last_error(e.what());
            return DIGITALCURLING_ERR_JSON_DATA;
        } catch (const uuidv7::invalid_format_error& e) {
            set_last_error(e.what());
            return DIGITALCURLING_ERR_UUID_PARSE;
        } catch (const digitalcurling::plugins::plugin_error& e) {
            set_last_error(e.what());
            return static_cast<DigitalCurling_ErrorCode>(e.code());
        } catch (const std::bad_alloc& e) {
            set_last_error(e.what());
            return DIGITALCURLING_ERR_MEMORY_ALLOCATION;
        } catch (const std::invalid_argument& e) {
            set_last_error(e.what());
            return DIGITALCURLING_ERR_INVALID_ARGUMENT;
        } catch (const std::exception& e) {
            set_last_error(e.what());
            return DIGITALCURLING_ERR_OTHER_EXCEPTION;
        } catch (...) {
            return DIGITALCURLING_ERR_UNKNOWN;
        }
    }

    struct SnapshotData {
        std::string data;

        explicit SnapshotData(std::string&& data_str) : data(std::move(data_str)) {}
    };
    class SnapshotManager {
    public:
        static SnapshotManager& GetInstance() {
            static SnapshotManager instance;
            return instance;
        }

        DigitalCurling_SnapshotHandle Create(std::string&& data) {
            auto data_ptr = std::make_shared<SnapshotData>(std::move(data));
            auto* raw_ptr = data_ptr.get();

            std::unique_lock lock(mutex_);
            valid_handles_[raw_ptr] = std::move(data_ptr);
            return reinterpret_cast<DigitalCurling_SnapshotHandle>(raw_ptr);
        }

        std::shared_ptr<SnapshotData> GetData(DigitalCurling_SnapshotHandle snapshot) {
            auto* data_ptr = reinterpret_cast<SnapshotData*>(snapshot);
            std::shared_lock lock(mutex_);
            auto it = valid_handles_.find(data_ptr);
            return it != valid_handles_.end() ? it->second : nullptr;
        }

        bool Destroy(DigitalCurling_SnapshotHandle snapshot) {
            auto* data_ptr = reinterpret_cast<SnapshotData*>(snapshot);
            std::shared_ptr<SnapshotData> sptr;
            {
                std::unique_lock lock(mutex_);
                auto it = valid_handles_.find(data_ptr);
                if (it == valid_handles_.end()) return false;

                sptr = std::move(it->second);
                valid_handles_.erase(it);
            }
            return true;
        }

    private:
        SnapshotManager() = default;
        ~SnapshotManager() {
            std::unique_lock lock(mutex_);
            for (auto& [handle, ptr] : valid_handles_) ptr.reset();
        }
        SnapshotManager(const SnapshotManager&) = delete;
        SnapshotManager& operator=(const SnapshotManager&) = delete;

        std::shared_mutex mutex_;
        std::unordered_map<SnapshotData*, std::shared_ptr<SnapshotData>> valid_handles_;
    };

    class InstanceManager {
    public:
        static InstanceManager& GetInstance() {
            static InstanceManager instance;
            return instance;
        }

        void Register(const uuidv7::uuidv7& id, std::weak_ptr<digitalcurling::plugins::PluginResource> plugin) {
            std::unique_lock lock(mutex_);
            map_.emplace(id, plugin);
        }

        template<PluginType Type>
        std::shared_ptr<typename digitalcurling::plugins::PluginTypeTraits<Type>::ResourceType> Get(const uuidv7::uuidv7& id) const {
            std::shared_lock lock(mutex_);
            auto it = map_.find(id);
            if (it == map_.end()) return nullptr;
            return std::dynamic_pointer_cast<typename digitalcurling::plugins::PluginTypeTraits<Type>::ResourceType>(it->second.lock());
        }

        bool Unregister(const uuidv7::uuidv7& id) {
            std::unique_lock lock(mutex_);
            return map_.erase(id) > 0;
        }
        void UnregisterAll(std::shared_ptr<digitalcurling::plugins::PluginResource> resource) {
            std::unique_lock lock(mutex_);
            auto ids = resource->GetInstanceList().GetAllId();
            for (auto& id : ids) { map_.erase(id); }
        }

    private:
        mutable std::shared_mutex mutex_;
        std::unordered_map<uuidv7::uuidv7, std::weak_ptr<digitalcurling::plugins::PluginResource>> map_;
    };
}

// --- Error Handling ---
DigitalCurling_ErrorCode dc_loader_get_last_error_message(char* out_buffer, size_t buffer_size, size_t* out_required_size) {
     if (!out_required_size) return DIGITALCURLING_ERR_INVALID_ARGUMENT;

    *out_required_size = last_error_message.length() + 1;
    if (!out_buffer) return DIGITALCURLING_OK;

    if (buffer_size < *out_required_size) return DIGITALCURLING_ERR_BUFFER_NULLPTR;
    std::strcpy(out_buffer, last_error_message.c_str());
    return DIGITALCURLING_OK;
}

// --- Snapshot ---
DigitalCurling_ErrorCode dc_loader_snapshot_get_data(DigitalCurling_SnapshotHandle snapshot, char* out_buffer, size_t buffer_size, size_t* out_required_size) {
    DIGITALCURLING_LOADER_CHECK_POINTER(snapshot);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_required_size);
    if (buffer_size > 0) DIGITALCURLING_LOADER_CHECK_POINTER(out_buffer);

    return catch_exceptions(__func__, [&]() {
        auto data_ptr = SnapshotManager::GetInstance().GetData(snapshot);
        if (!data_ptr)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Snapshot handle is not valid.");

        auto len = data_ptr->data.length();
        *out_required_size = len + 1;
        if (buffer_size <= 0) return DIGITALCURLING_OK;
        if (buffer_size < *out_required_size)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_BUFFER_NULLPTR, "out_buffer is too small.");

        std::memcpy(out_buffer, data_ptr->data.c_str(), len);
        out_buffer[len] = '\0';
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_destroy_snapshot(DigitalCurling_SnapshotHandle snapshot) {
    DIGITALCURLING_LOADER_CHECK_POINTER(snapshot);

    return catch_exceptions(__func__, [&]() {
        auto success = SnapshotManager::GetInstance().Destroy(snapshot);
        if (!success)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Snapshot handle is not valid.");
        return DIGITALCURLING_OK;
    });
}

// --- Core Plugin Management ---
DigitalCurling_ErrorCode dc_loader_load_plugin(const char* plugin_path, DigitalCurling_PluginType* out_plugin_type, char* out_plugin_name, size_t out_name_buffer_size) {
    DIGITALCURLING_LOADER_CHECK_POINTER(plugin_path);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_plugin_type);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_plugin_name);

    return catch_exceptions(__func__, [&]() {
        auto res = *PluginManager::GetInstance().LoadPlugin(plugin_path, true);
        *out_plugin_type = static_cast<DigitalCurling_PluginType>(res.type);
        if (res.name.length() >= out_name_buffer_size)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_BUFFER_NULLPTR, "out_plugin_name buffer is too small. (Loaded successfully)");

        std::strcpy(out_plugin_name, res.name.c_str());
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_get_loaded_plugins_snapshot(DigitalCurling_PluginType plugin_type, const char separator, DigitalCurling_SnapshotHandle* out_snapshot, size_t* out_snapshot_size) {
    DIGITALCURLING_LOADER_CHECK_POINTER(out_snapshot);

    return catch_exceptions(__func__, [&]() {
        auto type = static_cast<digitalcurling::plugins::PluginType>(plugin_type);
        auto plugins = PluginManager::GetInstance().GetLoadedPlugins(type);

        std::string concatenated;
        for (const auto& [type, name] : plugins) concatenated += name + separator;

        if (out_snapshot_size) *out_snapshot_size = concatenated.size() + 1;
        *out_snapshot = SnapshotManager::GetInstance().Create(std::move(concatenated));
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_is_plugin_loaded(DigitalCurling_PluginType plugin_type, const char* plugin_name, bool* out_loaded) {
    DIGITALCURLING_LOADER_CHECK_POINTER(plugin_name);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_loaded);

    return catch_exceptions(__func__, [&]() {
        auto type = static_cast<digitalcurling::plugins::PluginType>(plugin_type);
        *out_loaded = PluginManager::GetInstance().IsPluginLoaded(type, plugin_name);
        return DIGITALCURLING_OK;
    });
}

// --- Creator Common Functions ---
DigitalCurling_ErrorCode dc_loader_creator_get_state_snapshot(const DigitalCurling_Uuid* creator_id, DigitalCurling_SnapshotHandle* out_snapshot, size_t* out_snapshot_size) {
    DIGITALCURLING_LOADER_CHECK_POINTER(creator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_snapshot);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(creator_id->bytes);
        std::shared_ptr<digitalcurling::plugins::PluginResource> resource;
        if (auto player_res = InstanceManager::GetInstance().Get<PluginType::player>(uuid)) {
            resource = player_res;
        } else if (auto sim_res = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid)) {
            resource = sim_res;
        }

        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Creator instance not found.");

        auto result = resource->object_creator_get_state.ExecuteRaw(uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);

        auto snapshot_data = result.GetValue();
        if (out_snapshot_size) *out_snapshot_size = snapshot_data.size() + 1;
        *out_snapshot = SnapshotManager::GetInstance().Create(std::move(snapshot_data));
        return DIGITALCURLING_OK;
    });
}

// --- Player/Simulator Function Template ---
template <typename T>
DigitalCurling_ErrorCode create_plugin_object_impl(
    const char* func_name, const char* plugin_name, const char* json_config, DigitalCurling_Uuid* out_id
) {
    return catch_exceptions(func_name, [&]() {
        std::optional<uuidv7::uuidv7> uuid;
        std::shared_ptr<digitalcurling::plugins::PluginResource> resource;

        constexpr auto Type = digitalcurling::plugins::PluginTypeFor<T>::value;
        if constexpr (Type == PluginType::player) {
            resource = PluginManager::GetInstance().GetPlayerPluginResource(plugin_name);
            if (!resource) DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_PLUGIN_NOT_FOUND, "Player plugin not found.");
        } else if constexpr (Type == PluginType::simulator) {
            resource = PluginManager::GetInstance().GetSimulatorPluginResource(plugin_name);
            if (!resource) DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_PLUGIN_NOT_FOUND, "Simulator plugin not found.");
        } else {
            static_assert(digitalcurling::plugins::always_false<T>, "Unsupported Type");
        }

        if constexpr (std::is_base_of_v<digitalcurling::plugins::FactoryHandle, T>) {
            uuid = resource->create_factory.Execute(json_config);
        } else if constexpr (std::is_base_of_v<digitalcurling::plugins::StorageHandle, T>) {
            uuid = resource->create_storage.Execute(json_config);
        } else {
            static_assert(digitalcurling::plugins::always_false<T>, "Unsupported Type");
        }

        if (!uuid.has_value())
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_UNKNOWN, "Failed to create plugin object instance.");

        InstanceManager::GetInstance().Register(*uuid, resource);
        std::memcpy(out_id->bytes, uuid->get_bytes().data(), 16);
        return DIGITALCURLING_OK;
    });
}

template <PluginType Type>
DigitalCurling_ErrorCode target_load_impl(const char* func_name, const DigitalCurling_Uuid* target_id, const DigitalCurling_Uuid* storage_id) {
    return catch_exceptions(func_name, [&]() {
        auto target_uuid = uuidv7::uuidv7::from_bytes(target_id->bytes);
        auto storage_uuid = uuidv7::uuidv7::from_bytes(storage_id->bytes);

        auto resource = InstanceManager::GetInstance().Get<Type>(target_uuid);
        if (!resource) DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, digitalcurling::ToString(Type, true) + " plugin not found.");

        auto result = resource->load.ExecuteRaw(target_uuid, storage_uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        return DIGITALCURLING_OK;
    });
}
template <PluginType Type>
DigitalCurling_ErrorCode target_save_impl(const char* func_name, const DigitalCurling_Uuid* target_id, const DigitalCurling_Uuid* storage_id) {
    return catch_exceptions(func_name, [&]() {
        auto target_uuid = uuidv7::uuidv7::from_bytes(target_id->bytes);
        auto storage_uuid = uuidv7::uuidv7::from_bytes(storage_id->bytes);

        auto resource = InstanceManager::GetInstance().Get<Type>(target_uuid);
        if (!resource) DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, digitalcurling::ToString(Type, true) + " plugin not found.");

        auto result = resource->save.ExecuteRaw(target_uuid, storage_uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        return DIGITALCURLING_OK;
    });
}

// --- Player Instance Management ---
DigitalCurling_ErrorCode dc_loader_create_player_factory(const char* plugin_name, const char* json_config, DigitalCurling_Uuid* out_factory_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(plugin_name);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_factory_id);

    return create_plugin_object_impl<digitalcurling::players::IPlayerFactory>(
        __func__, plugin_name, json_config, out_factory_id
    );
}
DigitalCurling_ErrorCode dc_loader_create_player_storage(const char* plugin_name, const char* json_config, DigitalCurling_Uuid* out_storage_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(plugin_name);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_storage_id);

    return create_plugin_object_impl<digitalcurling::players::IPlayerStorage>(
        __func__, plugin_name, json_config, out_storage_id
    );
}
DigitalCurling_ErrorCode dc_loader_create_player(const DigitalCurling_Uuid* creator_id, DigitalCurling_Uuid* out_player_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(creator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_player_id);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(creator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::player>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Player creator instance not found.");

        auto result = resource->create_target.ExecuteRaw(uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);

        std::memcpy(out_player_id->bytes, result.GetValue().get_bytes().data(), 16);
        InstanceManager::GetInstance().Register(result.GetValue(), resource);
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_remove_player_instance(const DigitalCurling_Uuid* instance_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(instance_id);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(instance_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::player>(uuid);
        if (!resource || !resource->GetInstanceList().Remove(uuid))
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Player instance not found.");

        InstanceManager::GetInstance().Unregister(uuid);
        return DIGITALCURLING_OK;
    });
}

// --- Player Functions ---
DigitalCurling_ErrorCode dc_loader_player_load(const DigitalCurling_Uuid* player_id, const DigitalCurling_Uuid* storage_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(player_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(storage_id);
    return target_load_impl<PluginType::player>(__func__, player_id, storage_id);
}
DigitalCurling_ErrorCode dc_loader_player_save(const DigitalCurling_Uuid* player_id, const DigitalCurling_Uuid* storage_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(player_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(storage_id);
    return target_save_impl<PluginType::player>(__func__, player_id, storage_id);
}
DigitalCurling_ErrorCode dc_loader_player_get_gender(const DigitalCurling_Uuid* player_id, DigitalCurling_Gender* out_gender) {
    DIGITALCURLING_LOADER_CHECK_POINTER(player_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_gender);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(player_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::player>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Player plugin not found.");

        auto result = resource->get_gender.ExecuteRaw(uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        *out_gender = result.GetValue();
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_player_play(const DigitalCurling_Uuid* player_id, const DigitalCurling_Shot* shot_info, DigitalCurling_Shot* out_shot) {
    DIGITALCURLING_LOADER_CHECK_POINTER(player_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(shot_info);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_shot);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(player_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::player>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Player plugin not found.");

        auto result = resource->play.ExecuteRaw(uuid, shot_info);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        *out_shot = result.GetValue();
        return DIGITALCURLING_OK;
    });
}

// --- Simulator Instance Management ---
DigitalCurling_ErrorCode dc_loader_create_simulator_factory(const char* plugin_name, const char* json_config, DigitalCurling_Uuid* out_factory_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(plugin_name);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_factory_id);

    return create_plugin_object_impl<digitalcurling::simulators::ISimulatorFactory>(
        __func__, plugin_name, json_config, out_factory_id
    );
}
DigitalCurling_ErrorCode dc_loader_create_simulator_storage(const char* plugin_name, const char* json_config, DigitalCurling_Uuid* out_storage_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(plugin_name);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_storage_id);

    return create_plugin_object_impl<digitalcurling::simulators::ISimulatorStorage>(
        __func__, plugin_name, json_config, out_storage_id
    );
}
DigitalCurling_ErrorCode dc_loader_create_simulator(const DigitalCurling_Uuid* creator_id, DigitalCurling_Uuid* out_simulator_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(creator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_simulator_id);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(creator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator creator instance not found.");

        auto result = resource->create_target.ExecuteRaw(uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);

        std::memcpy(out_simulator_id->bytes, result.GetValue().get_bytes().data(), 16);
        InstanceManager::GetInstance().Register(result.GetValue(), resource);
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_remove_simulator_instance(const DigitalCurling_Uuid* instance_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(instance_id);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(instance_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource || !resource->GetInstanceList().Remove(uuid))
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator instance not found.");

        InstanceManager::GetInstance().Unregister(uuid);
        return DIGITALCURLING_OK;
    });
}

// --- Simulator Functions ---
DigitalCurling_ErrorCode dc_loader_simulator_load(const DigitalCurling_Uuid* simulator_id, const DigitalCurling_Uuid* storage_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(storage_id);
    return target_load_impl<PluginType::simulator>(__func__, simulator_id, storage_id);
}
DigitalCurling_ErrorCode dc_loader_simulator_save(const DigitalCurling_Uuid* simulator_id, const DigitalCurling_Uuid* storage_id) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(storage_id);
    return target_save_impl<PluginType::simulator>(__func__, simulator_id, storage_id);
}
DigitalCurling_ErrorCode dc_loader_simulator_set_stones(const DigitalCurling_Uuid* simulator_id, const DigitalCurling_StoneCoordinate* stones) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(stones);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(simulator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator instance not found.");

        auto result = resource->set_stones.ExecuteRaw(uuid, stones);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_simulator_step(const DigitalCurling_Uuid* simulator_id, const int frames, const float sheet_width) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(simulator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator instance not found.");

        auto result = resource->step.ExecuteRaw(uuid, frames, sheet_width);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_simulator_simulate(const DigitalCurling_Uuid* simulator_id, const DigitalCurling_SimulateModeFlag mode_flag, const float sheet_width) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(simulator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator instance not found.");

        auto result = resource->simulate.ExecuteRaw(uuid, mode_flag, sheet_width);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_simulator_get_stones(const DigitalCurling_Uuid* simulator_id, DigitalCurling_StoneCoordinate* out_stones) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_stones);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(simulator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator instance not found.");

        auto result = resource->get_stones.ExecuteRaw(uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        *out_stones = result.GetValue();
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_simulator_get_collisions_snapshot(const DigitalCurling_Uuid* simulator_id, DigitalCurling_SnapshotHandle* out_snapshot, size_t* out_snapshot_size) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_snapshot);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(simulator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator instance not found.");

        auto result = resource->get_collisions.ExecuteRaw(uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);

        auto snapshot_data = result.GetValue();
        if (out_snapshot_size) *out_snapshot_size = snapshot_data.size() + 1;
        *out_snapshot = SnapshotManager::GetInstance().Create(std::move(snapshot_data));
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_simulator_are_all_stones_stopped(const DigitalCurling_Uuid* simulator_id, bool* out_are_stopped) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_are_stopped);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(simulator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator instance not found.");

        auto result = resource->are_all_stones_stopped.ExecuteRaw(uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        *out_are_stopped = result.GetValue();
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_simulator_get_seconds_per_frame(const DigitalCurling_Uuid* simulator_id, float* out_seconds) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_seconds);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(simulator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator instance not found.");

        auto result = resource->get_seconds_per_frame.ExecuteRaw(uuid);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        *out_seconds = result.GetValue();
        return DIGITALCURLING_OK;
    });
}
DigitalCurling_ErrorCode dc_loader_simulator_calculate_shot(const DigitalCurling_Uuid* simulator_id, const DigitalCurling_Vector2* target_position,
                                                 float target_speed, float angular_velocity, DigitalCurling_Shot* out_shot) {
    DIGITALCURLING_LOADER_CHECK_POINTER(simulator_id);
    DIGITALCURLING_LOADER_CHECK_POINTER(target_position);
    DIGITALCURLING_LOADER_CHECK_POINTER(out_shot);

    return catch_exceptions(__func__, [&]() {
        auto uuid = uuidv7::uuidv7::from_bytes(simulator_id->bytes);
        auto resource = InstanceManager::GetInstance().Get<PluginType::simulator>(uuid);
        if (!resource)
            DIGITALCURLING_LOADER_RETURN_ERROR(DIGITALCURLING_ERR_INSTANCE_NOT_FOUND, "Simulator instance not found.");

        auto result = resource->calculate_shot.ExecuteRaw(uuid, target_position, target_speed, angular_velocity);
        DIGITALCURLING_LOADER_CHECK_PLUGIN_RESULT(result);
        *out_shot = result.GetValue();
        return DIGITALCURLING_OK;
    });
}