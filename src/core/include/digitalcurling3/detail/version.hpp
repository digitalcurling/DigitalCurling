// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief バージョン取得関数を定義

#pragma once

#include <cstdint>
#include <string>

namespace digitalcurling3 {

/// @brief バージョンを得る
/// @returns バージョン文字列(例: "1.0.0")
inline std::string GetVersion();

/// @brief メジャーバージョンを得る
/// @returns メジャーバージョン
inline std::uint32_t GetVersionMajor();

/// @brief マイナーバージョンを得る
/// @returns マイナーバージョン
inline std::uint32_t GetVersionMinor();

/// @brief パッチバージョンを得る
/// @returns パッチバージョン
inline std::uint32_t GetVersionPatch();

} // namespace digitalcurling3

#include "digitalcurling3/detail/version_info.hpp"
