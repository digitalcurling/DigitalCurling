#ifndef DIGITAL_CURLING_SRC_BOX2D_UTIL_HPP
#define DIGITAL_CURLING_SRC_BOX2D_UTIL_HPP

#include "box2d/box2d.h"
#include "digital_curling/detail/vector2.hpp"

namespace digital_curling {

inline b2Vec2 ToB2Vec2(Vector2 v)
{
    return { v.x, v.y };
}

inline Vector2 ToDC2Vector2(b2Vec2 v)
{
    return { v.x, v.y };
}

} // namespace digital_curling

#endif // DIGITAL_CURLING_SRC_BOX2D_UTIL_HPP
