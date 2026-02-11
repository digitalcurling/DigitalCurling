// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PluginLoader 用の C ABI 互換型を定義

#pragma once

#include "digitalcurling/plugins/data_object.h"

/// @addtogroup plugin_api
/// @{

/// @cond Doxygen_Suppress
typedef struct SnapshotData* DigitalCurling_SnapshotHandle;
/// @endcond

/// @brief UUIDを表す構造体
typedef struct {
    /// @brief 16バイトのUUIDデータ
    uint8_t bytes[16];
} DigitalCurling_Uuid;

/// @}
