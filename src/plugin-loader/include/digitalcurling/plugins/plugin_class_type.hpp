// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PluginClassType を定義

#pragma once

#include "digitalcurling/plugins/plugin_type.hpp"

namespace digitalcurling::plugins {

/// @brief プラグインクラスの種類
enum class PluginClassType {
    /// @brief Factory クラス
    factory = 0,
    /// @brief Storage クラス
    storage = 1,
    /// @brief Target クラス (Player/Simulator)
    target = 2
};

} // namespace digitalcurling::plugins
