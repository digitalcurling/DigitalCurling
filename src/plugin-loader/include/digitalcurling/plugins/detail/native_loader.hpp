// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>
#include <memory>
#include "digitalcurling/plugins/plugin_api.hpp"

namespace digitalcurling::plugins::detail {

using ModulePtr = std::unique_ptr<void, void(*)(void*)>;

struct NativePlugin {
    using GetPluginInfoFunc = PluginInfo (*)(void);
    using GetPluginApiFunc = PluginApi (*)(void);

    ModulePtr handle;
    GetPluginInfoFunc get_plugin_info;
    GetPluginApiFunc get_plugin_api;
};

NativePlugin LoadNativePlugin(const std::filesystem::path& path);

} // namespace digitalcurling::plugins::detail
