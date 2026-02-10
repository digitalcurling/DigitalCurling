// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#pragma once

#include "box2d/box2d.h"
#include "digitalcurling3/detail/vector2.hpp"

namespace digitalcurling3 {

inline b2Vec2 ToB2Vec2(Vector2 v)
{
    return { v.x, v.y };
}

inline Vector2 ToDC2Vector2(b2Vec2 v)
{
    return { v.x, v.y };
}

} // namespace digitalcurling3
