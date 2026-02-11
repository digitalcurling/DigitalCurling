// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief 座標変換関連を定義

#pragma once

#include <cassert>
#include "vector2.hpp"

namespace digitalcurling::coordinate {

namespace detail {

/// @cond Doxygen_Suppress
constexpr float kHogLineYOnSimulation = 10.9725f;
constexpr float kTeeLineYOnSimulation = 17.3735f;
constexpr float kBackLineYOnSimulation = 19.2025f;
constexpr float kHackYOnSimulation = 21.0315f;
constexpr float kBackBoardYOnSimulation = 22.8605f;
/// @endcond

} // namespace detail



/// @brief ハウスの半径
constexpr float kHouseRadius = 1.829f;



/// @brief 座標系ID
enum class Id {
    /// @brief シミュレーション座標系
    /// アイスシート中央を原点，サイド0でストーンを投げる方向をy軸正方向とする．
    kSimulation,

    /// @brief ショット座標系(サイド0)
    /// シミュレーション座標系でy座標が負のハック(ストーン射出位置)を原点，そこからストーンを投げる方向をy軸正方向とする．
    kShot0,

    /// @brief ショット座標系(サイド1)
    /// シミュレーション座標系でy座標が正のハック(ストーン射出位置)を原点，そこからストーンを投げる方向をy軸正方向とする．
    kShot1,
};



/// @brief エンドに対応するショット座標系を返す
/// @param[in] end エンド番号(0スタート)
/// @returns Id::kShot0 または Id::kShot1
constexpr Id GetShotSide(std::uint8_t end) noexcept
{
    if (end % 2 == 0) {
        return Id::kShot0;
    } else {
        return Id::kShot1;
    }
}



/// @brief 位置を座標変換する
/// @param[in] position @a from で示された座標系上での座標
/// @param[in] from @a position の座標系
/// @param[in] to 変換先の座標系
/// @returns @a to 座標系上での位置
constexpr Vector2 TransformPosition(Vector2 position, Id from, Id to) noexcept
{
    if (from == to) return position;

    switch (to) {
        case Id::kSimulation:
            switch (from) {
                case Id::kShot0:
                    return { position.x, position.y - detail::kHackYOnSimulation };
                case Id::kShot1:
                    return { -position.x, -position.y + detail::kHackYOnSimulation };
                default:
                    assert(false);
                    return Vector2();
            }
            break;

        case Id::kShot0:
            switch (from) {
                case Id::kSimulation:
                    return { position.x, position.y + detail::kHackYOnSimulation };
                case Id::kShot1:
                    return { -position.x, -position.y + 2 * detail::kHackYOnSimulation };
                default:
                    assert(false);
                    return Vector2();
            }
            break;

        case Id::kShot1:
            switch (from) {
                case Id::kSimulation:
                    return { -position.x, -position.y + detail::kHackYOnSimulation };
                case Id::kShot0:
                    return { -position.x, -position.y + 2 * detail::kHackYOnSimulation };
                default:
                    assert(false);
                    return Vector2();
            }
            break;

        default:
            assert(false);
            return Vector2();
    }
}



/// @brief 速度を座標系間で変換する
/// @param[in] velocity @a from で示された座標系上での速度
/// @param[in] from @a velocity の座標系
/// @param[in] to 変換先の座標系
/// @returns @a to 座標系上での速度
constexpr Vector2 TransformVelocity(Vector2 velocity, Id from, Id to) noexcept
{
    if (from == to) return velocity;

    // 軸の方向が変化するのはどちらかがKShot1の場合のみ
    if (from == Id::kShot1 || to == Id::kShot1) {
        return -velocity;
    }
    return velocity;
}



/// @brief 角度を座標系間で変換する
/// @param[in] angle @a from で示された座標系上での角度
/// @param[in] from @a angleの座標系
/// @param[in] to 変換先の座標系
/// @returns @a to 座標系上での角度
constexpr float TransformAngle(float angle, Id from, Id to) noexcept
{
    if (from == to) return angle;

    // 軸の方向が変化するのはfrom/toのどちらかがkShot1の場合のみ
    if (from == Id::kShot1) {
        return angle - kPi;
    }
    if (to == Id::kShot1) {
        return angle + kPi;
    }
    return angle;
}



/// @brief 角速度を座標系間で変換する
/// @param[in] angular_velocity @a from で示された座標系上での角速度
/// @param[in] from @a angular_velocity の座標系
/// @param[in] to 変換先の座標系
/// @returns @a to 座標系上での角速度
constexpr float TransformAngularVelocity(float angular_velocity, Id from, Id to) noexcept
{
    // 角速度は座標系が変化しても同じ
    return angular_velocity;
}



/// @brief センターラインのx座標を得る
/// @param[in] coord 座標系
/// @returns @a coord 座標系のセンターラインのx座標
constexpr float GetCenterLineX(Id coord) noexcept
{
    return 0.f;
}



/// @brief ホグラインのy座標を得る
/// @param[in] side `false` : シミュレーション座標系で負の側, `true` : シミュレーション座標系で正の側
/// @param[in] coord 座標系
/// @returns @a coord 座標系のホグラインのy座標
constexpr float GetHogLineY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kHogLineYOnSimulation : -detail::kHogLineYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



/// @brief ティーラインのy座標を得る
/// @param[in] side `false` : シミュレーション座標系で負の側, `true` : シミュレーション座標系で正の側
/// @param[in] coord 座標系
/// @returns @a coord 座標系のティーラインのy座標
constexpr float GetTeeLineY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kTeeLineYOnSimulation : -detail::kTeeLineYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



/// @brief バックラインのy座標を得る
/// @param[in] side `false` : シミュレーション座標系で負の側, `true` : シミュレーション座標系で正の側
/// @param[in] coord 座標系
/// @returns @a coord 座標系のバックラインのy座標
constexpr float GetBackLineY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kBackLineYOnSimulation : -detail::kBackLineYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



/// @brief ハックのy座標を得る
/// @param[in] side `false` : シミュレーション座標系で負の側, `true` : シミュレーション座標系で正の側
/// @param[in] coord 座標系
/// @returns @a coord 座標系のハックのy座標
constexpr float GetHackY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kHackYOnSimulation : -detail::kHackYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



/// @brief バックボード(シートの端)のy座標を得る
/// @param[in] side `false` : シミュレーション座標系で負の側, `true` : シミュレーション座標系で正の側
/// @param[in] coord 座標系
/// @returns @a coord 座標系のバックボードのy座標
constexpr float GetBackBoardY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kBackBoardYOnSimulation : -detail::kBackBoardYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



} // namespace digitalcurling::coordinate
