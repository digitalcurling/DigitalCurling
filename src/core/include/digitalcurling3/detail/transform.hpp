// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief Transform を定義します

#pragma once

#include "vector2.hpp"
#include "nlohmann/json.hpp"

namespace digitalcurling3 {



/// \brief 位置，角度を格納します．
struct Transform {
    Vector2 position;  ///< 位置
    float angle;       ///< 角度

    /// \brief 位置(0, 0)，角度0で初期化します
    constexpr Transform() : position(), angle(0.f) {}

    /// \brief 指定された値で初期化します
    ///
    /// \param[in] position 位置
    /// \param[in] angle 角度
    constexpr Transform(Vector2 position, float angle) : position(position), angle(angle) {}
};


// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Transform,
    position,
    angle
)

} // namespace digitalcurling3
