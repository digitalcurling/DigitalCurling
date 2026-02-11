// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#pragma once

#include <cmath>
#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <nlohmann/json.hpp>
#include "digitalcurling/plugins/i_plugin_object.hpp"
#include "digitalcurling/plugins/data_object.h"
#include "digitalcurling/plugins/plugin_api.hpp"
#include "digitalcurling/plugins/plugin_type.hpp"


//* NOTE: DIGITALCURLING_PLUGIN_NAME must be defined as UTF-8 string literal before including this file.
#ifndef DIGITALCURLING_PLUGIN_NAME
    #error "DIGITALCURLING_PLUGIN_NAME must be defined before including plugin_exports.hpp"
#endif

#if defined(_WIN32)
    #define DIGITALCURLING_EXPORT extern "C" __declspec(dllexport)
#else
    #define DIGITALCURLING_EXPORT extern "C" __attribute__((visibility("default")))
#endif

namespace digitalcurling::plugins::detail {

// --- Error/Exception handling functions ---
inline char* DuplicateString(const std::string& str)
{
    char* copy = static_cast<char*>(malloc(str.length() + 1));
    if (copy) memcpy(copy, str.c_str(), str.length() + 1);
    return copy;
}

inline DigitalCurling_ErrorCode ReturnError(const DigitalCurling_ErrorCode type, const std::string& message, char** out_error)
{
    if (!out_error) return DIGITALCURLING_ERR_BUFFER_NULLPTR;

    *out_error = DuplicateString(message);
    return *out_error ? type : DIGITALCURLING_ERR_MEMORY_ALLOCATION;
}

inline DigitalCurling_ErrorCode ReturnException(const std::exception& e, const std::string& function_name, char** out_error)
{
    DigitalCurling_ErrorCode type;
    if (dynamic_cast<const nlohmann::json::parse_error*>(&e)) {
        type = DIGITALCURLING_ERR_JSON_PARSE;
    } else if (dynamic_cast<const nlohmann::json::exception*>(&e)) {
        type = DIGITALCURLING_ERR_JSON_DATA;
    } else if (dynamic_cast<const std::bad_alloc*>(&e)) {
        type = DIGITALCURLING_ERR_MEMORY_ALLOCATION;
    } else if (dynamic_cast<const std::invalid_argument*>(&e)) {
        type = DIGITALCURLING_ERR_INVALID_ARGUMENT;
    } else if (dynamic_cast<const std::exception*>(&e)) {
        type = DIGITALCURLING_ERR_OTHER_EXCEPTION;
    } else {
        type = DIGITALCURLING_ERR_UNKNOWN;
    }
    return ReturnError(type, function_name + ": " + e.what(), out_error);
}

inline DigitalCurling_ErrorCode ReturnString(const std::string& value, const std::string& function_name, char** out_str, char** out_error) {
    if (!out_str) {
        return ReturnError(DIGITALCURLING_ERR_BUFFER_NULLPTR, function_name + ": result buffer is nullptr.", out_error);
    }
    *out_str = DuplicateString(value);
    return *out_str ? DIGITALCURLING_OK :
        ReturnError(DIGITALCURLING_ERR_MEMORY_ALLOCATION, function_name + ": Memory allocation failed.", out_error);
}


// --- C++ Implementation to C-ABI Wrapper Functions ---
inline void FreeStringImpl(char* str)
{
    free(str);
}
template <typename T>
void DestroyObjectImpl(void* handle)
{
    delete static_cast<T*>(handle);
}

template <typename T, typename THandlePtr>
DigitalCurling_ErrorCode CreateCreatorImpl(const char* json, THandlePtr* out_handle, char** out_error)
{
    if (!out_handle)
        return ReturnError(DIGITALCURLING_ERR_BUFFER_NULLPTR, "CreateCreator: out_handle is nullptr.", out_error);

    try {
        auto creator = std::make_unique<T>();
        if (json && *json != '\0') {
            nlohmann::json j = nlohmann::json::parse(json);
            from_json(j, *creator);
        }

        static_assert(std::is_convertible<T*, THandlePtr>::value, "CreateCreator: T* is not convertible to THandlePtr.");
        *out_handle = static_cast<THandlePtr>(creator.release());
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "CreateCreator", out_error);
    }
}
//template <typename T>
inline DigitalCurling_ErrorCode CreateTargetImpl(CreatorHandle* creator, TargetHandle** out_target_handle, char** out_error)
{
    if (!creator)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "CreateTarget: creator handle is nullptr.", out_error);
    if (!out_target_handle)
        return ReturnError(DIGITALCURLING_ERR_BUFFER_NULLPTR, "CreateTarget: out_target_handle is nullptr.", out_error);

    try {
        auto* creator_ptr = dynamic_cast<typename digitalcurling::plugins::IPluginObjectCreator*>(creator);
        if (!creator_ptr)
            return ReturnError(DIGITALCURLING_ERR_FAILED_TO_CAST, "CreateTarget: creator handle is not of type IPluginObjectCreator.", out_error);

        *out_target_handle = creator_ptr->Create();
        return *out_target_handle ? DIGITALCURLING_OK : DIGITALCURLING_ERR_MEMORY_ALLOCATION;
    } catch (const std::exception& e) {
        return ReturnException(e, "CreateTarget", out_error);
    }
}

inline DigitalCurling_ErrorCode CreatorGetStateImpl(CreatorHandle* creator, char** out_json, char** out_error)
{
    if (!creator)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "CreatorGetState: creator handle is nullptr.", out_error);

    try {
        auto* creator_ptr = dynamic_cast<typename digitalcurling::plugins::IPluginObjectCreator*>(creator);
        return ReturnString(creator_ptr->ToJson().dump(), "CreatorGetState", out_json, out_error);
    } catch (const std::exception& e) {
        return ReturnException(e, "CreatorGetState", out_error);
    }
}
template <typename T, typename THandle>
DigitalCurling_ErrorCode CreatorSetStateImpl(THandle creator, const char* json, char** out_error)
{
    if (!creator)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "CreatorSetState: creator handle is nullptr.", out_error);
    if (!json)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "CreatorSetState: json is nullptr.", out_error);

    try {
        auto* creator_ptr = dynamic_cast<T*>(creator);
        auto j = creator_ptr->ToJson();
        j.merge_patch(nlohmann::json::parse(json));
        from_json(j, *creator_ptr);
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "CreatorSetState", out_error);
    }
}

template <PluginType Type, typename Factory>
DigitalCurling_ErrorCode GetFactoryImpl(typename digitalcurling::plugins::PluginTypeTraits<Type>::HandleType* handle, FactoryHandle** out_factory_handle, char** out_error)
{
    if (!handle)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "GetFactory: handle is nullptr.", out_error);
    if (!out_factory_handle)
        return ReturnError(DIGITALCURLING_ERR_BUFFER_NULLPTR, "GetFactory: out_factory_handle is nullptr.", out_error);

    try {
        const auto& factory_ref = dynamic_cast<typename digitalcurling::plugins::PluginTypeTraits<Type>::TargetType*>(handle)->GetFactory();
        *out_factory_handle = static_cast<digitalcurling::plugins::FactoryHandle*>(new Factory(dynamic_cast<const Factory&>(factory_ref)));
        return *out_factory_handle ? DIGITALCURLING_OK : DIGITALCURLING_ERR_MEMORY_ALLOCATION;
    } catch (const std::exception& e) {
        return ReturnException(e, "GetFactory", out_error);
    }
}
template <PluginType Type, typename Storage>
DigitalCurling_ErrorCode TargetSaveImpl(typename digitalcurling::plugins::PluginTypeTraits<Type>::HandleType* handle, StorageHandle* storage_handle, char** out_error)
{
    if (!handle)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "Save: handle is nullptr.", out_error);
    if (!storage_handle)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "Save: storage handle is nullptr.", out_error);

    try {
        dynamic_cast<typename digitalcurling::plugins::PluginTypeTraits<Type>::TargetType*>(handle)->Save(*dynamic_cast<Storage*>(storage_handle));
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "Save", out_error);
    }
}
template <PluginType Type, typename Storage>
DigitalCurling_ErrorCode TargetLoadImpl(typename digitalcurling::plugins::PluginTypeTraits<Type>::HandleType* handle, StorageHandle* storage_handle, char** out_error)
{
    if (!handle)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "Load: handle is nullptr.", out_error);
    if (!storage_handle)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "Load: storage handle is nullptr.", out_error);

    try {
        dynamic_cast<typename digitalcurling::plugins::PluginTypeTraits<Type>::TargetType*>(handle)->Load(*dynamic_cast<const Storage*>(storage_handle));
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "Load", out_error);
    }
}

} // namespace


namespace dcpd = digitalcurling::plugins::detail;

// --- Internal Macro for Exporting the Plugin API Function ---
#ifdef DIGITALCURLING_BUNDLE_PLUGINS
    struct StaticPluginRegistrar {
        StaticPluginRegistrar(digitalcurling::plugins::PluginInfo info, digitalcurling::plugins::PluginApi api) {
            digitalcurling::plugins::RegisterStaticPlugin(info, api);
        }
    };

    #define DIGITALCURLING_EXPORT_PLUGIN_FUNCTIONS(InfoInstance, ApiInstance) \
        namespace { \
            StaticPluginRegistrar g_static_registrar(InfoInstance, ApiInstance); \
        }
#else
    #define DIGITALCURLING_EXPORT_PLUGIN_FUNCTIONS(InfoInstance, ApiInstance) \
        DIGITALCURLING_EXPORT digitalcurling::plugins::PluginInfo dc_get_plugin_info() \
        { \
            return InfoInstance; \
        } \
        DIGITALCURLING_EXPORT digitalcurling::plugins::PluginApi dc_get_plugin_api() \
        { \
            return ApiInstance; \
        }
#endif // DIGITALCURLING_BUNDLE_PLUGINS

#define DIGITALCURLING_EXPORT_PLUGIN_INNER(PluginType, FactoryClass, StorageClass, PlayerClass, PlayerApiInstance, SimulatorApiInstance) \
    static const digitalcurling::plugins::PluginInfo g_plugin_info = { \
        /*plugin_name*/ DIGITALCURLING_PLUGIN_NAME, \
        /*plugin_type*/ static_cast<DigitalCurling_PluginType>(PluginType), \
        /*plugin_version*/ DIGITALCURLING_PLUGIN_API_VERSION \
    }; \
    \
    static const digitalcurling::plugins::PluginApi g_plugin_api = { \
        /*free_string*/ &dcpd::FreeStringImpl, \
        \
        /*create_factory*/ &dcpd::CreateCreatorImpl<FactoryClass, digitalcurling::plugins::FactoryHandle*>, \
        /*create_storage*/ &dcpd::CreateCreatorImpl<StorageClass, digitalcurling::plugins::StorageHandle*>, \
        /*destroy_factory*/ &dcpd::DestroyObjectImpl<FactoryClass>, \
        /*destroy_storage*/ &dcpd::DestroyObjectImpl<StorageClass>, \
        /*create_target*/ &dcpd::CreateTargetImpl, \
        /*destroy_target*/ &dcpd::DestroyObjectImpl<PlayerClass>, \
        \
        /*object_creator_get_state*/ &dcpd::CreatorGetStateImpl, \
        /*factory_set_state*/ &dcpd::CreatorSetStateImpl<FactoryClass, digitalcurling::plugins::FactoryHandle*>, \
        /*storage_set_state*/ &dcpd::CreatorSetStateImpl<StorageClass, digitalcurling::plugins::StorageHandle*>, \
        \
        /*player*/ PlayerApiInstance, \
        /*simulator*/ SimulatorApiInstance \
    }; \
    DIGITALCURLING_EXPORT_PLUGIN_FUNCTIONS(g_plugin_info, g_plugin_api)
