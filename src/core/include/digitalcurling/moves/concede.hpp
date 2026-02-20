// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief Concede を定義

#pragma once

#include <string_view>

namespace digitalcurling::moves {


/// @brief 行動：コンシード
/// @sa Move
struct Concede {
    /// @brief `Move` の識別用タイプ名
    static constexpr std::string_view kTypeName = "concede";
};

} // namespace digitalcurling::moves
