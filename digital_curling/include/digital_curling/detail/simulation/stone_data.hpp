#ifndef DIGITAL_CURLING_SIMULATION_STONE_DATA_HPP
#define DIGITAL_CURLING_SIMULATION_STONE_DATA_HPP

#include <array>
#include <optional>
#include "../vector2.hpp"
#include "../constants.hpp"

namespace digital_curling::simulation {



/// <summary>
/// ストーンの位置と速度
/// </summary>
struct StoneData {

    /// <summary>
    /// ストーンの中心の座標 [m]
    /// </summary>
    Vector2 position;

    /// <summary>
    /// 角度 [radian]
    /// </summary>
    float angle;

    /// <summary>
    /// 速度 [m/s]
    /// </summary>
    Vector2 linear_velocity;

    /// <summary>
    /// 角速度 [radian/s]
    /// </summary>
    float angular_velocity;

    StoneData()
        : position()
        , angle(0.f)
        , linear_velocity()
        , angular_velocity(0.f) {}

    StoneData(Vector2 position, float angle, Vector2 linear_velocity, float angular_velocity)
        : position(position)
        , angle(angle)
        , linear_velocity(linear_velocity)
        , angular_velocity(angular_velocity) {}
};



/// <summary>
/// 全てのストーンの位置と速度。盤面に存在しないストーンはstd::nulloptとして表される。
/// </summary>
using AllStoneData = std::array<std::optional<StoneData>, kStoneMax>;



} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SIMULATION_STONE_DATA_HPP
