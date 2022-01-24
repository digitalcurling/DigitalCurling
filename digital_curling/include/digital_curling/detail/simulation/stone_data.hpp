/// \file
/// \brief StoneData, AllStoneData を定義します

#ifndef DIGITAL_CURLING_SIMULATION_STONE_DATA_HPP
#define DIGITAL_CURLING_SIMULATION_STONE_DATA_HPP

#include <array>
#include <optional>
#include "../vector2.hpp"
#include "../constants.hpp"

namespace digital_curling::simulation {



/// \brief ストーンの位置，角度，速度，角速度を格納します
struct StoneData {

    Vector2 position; ///< ストーンの中心の座標(m)

    float angle; ///< 角度(radian)

    Vector2 linear_velocity; ///< 速度(m/s)

    float angular_velocity; ///< 角速度(radian/s)

    /// \brief 全データを0で初期化します
    StoneData()
        : position()
        , angle(0.f)
        , linear_velocity()
        , angular_velocity(0.f) {}

    /// \brief 与えられたデータで初期化します
    ///
    /// \param[in] position 位置(m)
    /// \param[in] angle 角度(radian)
    /// \param[in] linear_velocity 速度(m/s)
    /// \param[in] angular_velocity 角速度(radian/s)
    StoneData(Vector2 position, float angle, Vector2 linear_velocity, float angular_velocity)
        : position(position)
        , angle(angle)
        , linear_velocity(linear_velocity)
        , angular_velocity(angular_velocity) {}
};



/// \brief 全ストーンの位置と速度
///
/// 盤面に存在しないストーンは \c std::nullopt として表されます．
using AllStoneData = std::array<std::optional<StoneData>, kStoneMax>;



} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SIMULATION_STONE_DATA_HPP
