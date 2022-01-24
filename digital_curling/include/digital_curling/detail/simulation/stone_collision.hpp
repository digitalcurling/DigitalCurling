/// \file
/// \brief StoneCollision を定義します

#ifndef DIGITAL_CURLING_SIMULATION_STONE_COLLISION_HPP
#define DIGITAL_CURLING_SIMULATION_STONE_COLLISION_HPP

#include <cstdint>
#include "../vector2.hpp"

namespace digital_curling::simulation {



/// \brief ストーンどうしの衝突の情報
struct StoneCollision {

    std::uint8_t a_id; ///< ストーンAのID

    std::uint8_t b_id; ///< ストーンBのID

    Vector2 a_position; ///< ストーンAの位置

    Vector2 b_position; ///< ストーンBの位置

    float normal_impulse; ///< 法線方向の撃力

    float tangent_impulse; ///< 接線方向の撃力

    /// \brief 全パラメータを 0 で初期化します
    StoneCollision()
        : a_id(0)
        , b_id(0)
        , a_position()
        , b_position()
        , normal_impulse(0.f)
        , tangent_impulse(0.f) {}

    /// \brief 与えられたパラメータで初期化します
    ///
    /// \param[in] a_id ストーンAのID
    /// \param[in] b_id ストーンBのID
    /// \param[in] a_position ストーンAの位置
    /// \param[in] b_position ストーンBの位置
    /// \param[in] normal_impulse 法線方向の撃力
    /// \param[in] tangent_impulse 接線方向の撃力
    StoneCollision(std::uint8_t a_id, std::uint8_t b_id, Vector2 a_position, Vector2 b_position, float normal_impulse, float tangent_impulse)
        : a_id(a_id)
        , b_id(b_id)
        , a_position(a_position)
        , b_position(b_position)
        , normal_impulse(normal_impulse)
        , tangent_impulse(tangent_impulse) {}

    /// \brief ストーンどうしが接した座標を得る。
    ///
    /// \returns ストーンどうしが接した座標
    Vector2 GetContactPoint() const
    {
        return (a_position + b_position) * 0.5f;
    }
};



} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SIMULATION_STONE_COLLISION_HPP
