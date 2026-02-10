// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief Concede を定義します

#ifndef DIGITALCURLING3_MOVES_CONCEDE_HPP
#define DIGITALCURLING3_MOVES_CONCEDE_HPP

#include <string_view>

namespace digitalcurling3::moves {


/// \brief 行動：コンシード
///
struct Concede {
    /// \brief `Move` の識別用タイプ名
    static constexpr std::string_view kTypeName = "concede";
};

} // namespace digitalcurling3::moves

#endif // DIGITALCURLING3_MOVES_CONCEDE_HPP
