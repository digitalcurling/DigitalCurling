// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief IPlayer を定義

#pragma once

#include <nlohmann/json.hpp>

namespace digitalcurling::players {

/// @brief プレイヤーの性別
enum class Gender : int8_t {
    /// @brief 男性
    kMale,
    /// @brief 女性
    kFemale,
    /// @brief 性別不明
    kUnknown = -1,
};

/// @cond Doxygen_Suppress
NLOHMANN_JSON_SERIALIZE_ENUM(Gender, {
    {Gender::kMale, "male"},
    {Gender::kFemale, "female"},
    {Gender::kUnknown, "unknown"},
})
/// @endcond

} // namespace digitalcurling::players
