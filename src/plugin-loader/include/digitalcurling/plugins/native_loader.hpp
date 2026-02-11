// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @cond Doxygen_Suppress

#pragma once

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #define NOMINMAX

    #include <windows.h>
    using ModulePtr = HMODULE;
#else
    #include <dlfcn.h>
    using ModulePtr = void*;
#endif

#include <filesystem>
#include "digitalcurling/plugins/plugin_api.hpp"

namespace digitalcurling::plugins {


using GetPluginInfoFunc = PluginInfo (*)(void);
using GetPluginApiFunc = PluginApi (*)(void);

struct PluginExportedFunctions {
    GetPluginInfoFunc get_plugin_info;
    GetPluginApiFunc get_plugin_api;
};

ModulePtr LoadSharedLibrary(const std::filesystem::path& path);
PluginExportedFunctions GetPluginExportedFunctions(ModulePtr module);
bool CloseSharedLibrary(ModulePtr module);

} // namespace digitalcurling::plugins

/// @endcond
