#ifndef DIGITAL_CURLING_SIMULATION_STONE_COLLISION_HPP
#define DIGITAL_CURLING_SIMULATION_STONE_COLLISION_HPP

#include <cstdint>
#include "../vector2.hpp"

namespace digital_curling::simulation {



/// <summary>
/// ストーンどうしの衝突の情報
/// </summary>
struct StoneCollision {

    /// <summary>
    /// ストーンAのID
    /// </summary>
    std::uint8_t a_id;

    /// <summary>
    /// ストーンBのID
    /// </summary>
    std::uint8_t b_id;

    /// <summary>
    /// ストーンAの位置
    /// </summary>
    Vector2 a_position;

    /// <summary>
    /// ストーンBの位置
    /// </summary>
    Vector2 b_position;

    /// <summary>
    /// 法線方向の撃力
    /// </summary>
    float normal_impulse;

    /// <summary>
    /// 接線方向の撃力
    /// </summary>
    float tangent_impulse;

    StoneCollision()
        : a_id(0)
        , b_id(0)
        , a_position()
        , b_position()
        , normal_impulse(0.f)
        , tangent_impulse(0.f) {}

    StoneCollision(std::uint8_t a_id, std::uint8_t b_id, Vector2 a_position, Vector2 b_position, float normal_impulse, float tangent_impulse)
        : a_id(a_id)
        , b_id(b_id)
        , a_position(a_position)
        , b_position(b_position)
        , normal_impulse(normal_impulse)
        , tangent_impulse(tangent_impulse) {}

    /// <summary>
    /// ストーンどうしが接した座標を得る。
    /// </summary>
    /// <returns>ストーンどうしが接した座標</returns>
    Vector2 GetContactPoint() const
    {
        return (a_position + b_position) * 0.5f;
    }
};



} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SIMULATION_STONE_COLLISION_HPP
