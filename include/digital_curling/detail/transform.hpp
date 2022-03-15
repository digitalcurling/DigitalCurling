/// \file
/// \brief Transform を定義します

#ifndef DIGITAL_CURLING_TRANSFORM_HPP
#define DIGITAL_CURLING_TRANSFORM_HPP

#include "vector2.hpp"
#include "nlohmann/json.hpp"

namespace digital_curling {



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

} // namespace digital_curling

#endif // DIGITAL_CURLING_TRANSFORM_HPP
