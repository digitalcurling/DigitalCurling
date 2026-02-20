// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#pragma once

#include <box2d/box2d.h>
#include "digitalcurling/vector2.hpp"

namespace digitalcurling::simulators {


/// @brief Box2D の b2Vec2 に変換する
/// @param v ベクトル (digitalcurling::Vector2)
/// @return ベクトル (b2Vec2)
inline b2Vec2 ToB2Vec2(Vector2 v)
{
    return { v.x, v.y };
}
/// @brief DigitalCurling の Vector2 に変換する
/// @param v ベクトル (b2Vec2)
/// @return ベクトル (digitalcurling::Vector2)
inline Vector2 ToDigitalCurlingVector2(b2Vec2 v)
{
    return { v.x, v.y };
}

} // namespace digitalcurling::simulators
