#ifndef DIGITAL_CURLING_COORDINATE_HPP
#define DIGITAL_CURLING_COORDINATE_HPP

#include "vector2.hpp"

namespace digital_curling::coordinate {

namespace detail {

    constexpr float kHogLineYOnSimulation = 10.9725f;
    constexpr float kTeeLineYOnSimulation = 17.3735f;
    constexpr float kBackLineYOnSimulation = 19.2025f;
    constexpr float kHackYOnSimulation = 21.0315f;
    constexpr float kBackBoardYOnSimulation = 22.8605f;

} // namespace detail



/// <summary>
/// ハウスの半径
/// </summary>
constexpr float kHouseRadius = 1.829f;



/// <summary>
/// 座標系ID
/// </summary>
enum class Id {
    /// <summary>
    /// シミュレーション座標系．アイスシート中央を原点，サイド0のハウスがある方向をy軸正方向とする．
    /// </summary>
    kSimulation,

    /// <summary>
    /// ショット座標系(サイド0)．シミュレーション座標系でy座標が負のハック(ストーン射出位置)を原点，そこからストーンを投げる方向をy軸正方向とする．
    /// </summary>
    kShot0,

    /// <summary>
    /// ショット座標系(サイド1)．シミュレーション座標系でy座標が正のハック(ストーン射出位置)を原点，そこからストーンを投げる方向をy軸正方向とする．
    /// </summary>
    kShot1,
};



/// <summary>
/// エンドに対応するシートサイドを返す．
/// </summary>
/// <param name="current_end">エンド</param>
/// <returns><see cref="Id::kShot0"/>または<see cref="Id::kShot1"/></returns>
constexpr Id GetShotSide(std::uint8_t end) noexcept
{
    if (end % 2 == 0) {
        return Id::kShot0;
    } else {
        return Id::kShot1;
    }
}



/// <summary>
/// 位置を座標変換する．
/// </summary>
/// <param name="position"><paramref name="from"/>で示された座標系上での座標</param>
/// <param name="from"><paramref name="position"/>の座標系</param>
/// <param name="to">変換先の座標系</param>
/// <returns><paramref name="to"/>座標系上での位置</returns>
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



/// <summary>
/// 速度を座標系間で変換する．
/// </summary>
/// <param name="velocity"><paramref name="from"/>で示された座標系上での速度</param>
/// <param name="from"><paramref name="velocity"/>の座標系</param>
/// <param name="to">変換先の座標系</param>
/// <returns><paramref name="to"/>座標系上での速度</returns>
constexpr Vector2 TransformVelocity(Vector2 velocity, Id from, Id to) noexcept
{
    if (from == to) return velocity;

    // 軸の方向が変化するのはどちらかがKShot1の場合のみ
    if (from == Id::kShot1 || to == Id::kShot1) {
        return -velocity;
    }
    return velocity;
}



/// <summary>
/// 角度を座標系間で変換する．
/// </summary>
/// <param name="angle"><paramref name="from"/>で示された座標系上での角度</param>
/// <param name="from"><paramref name="angle"/>の座標系</param>
/// <param name="to">変換先の座標系</param>
/// <returns><paramref name="to"/>座標系上での角度</returns>
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



/// <summary>
/// 角速度を座標系間で変換する．
/// </summary>
/// <param name="angular_velocity"><paramref name="from"/>で示された座標系上での角速度</param>
/// <param name="from"><paramref name="angular_velocity"/>の座標系</param>
/// <param name="to">変換先の座標系</param>
/// <returns><paramref name="to"/>座標系上での角速度</returns>
constexpr float TransformAngularVelocity(float angular_velocity, Id from, Id to) noexcept
{
    // 角速度は座標系が変化しても同じ
    return angular_velocity;
}



/// <summary>
/// センターラインのx座標を得る．
/// </summary>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のセンターラインのx座標</returns>
constexpr float GetCenterLineX(Id coord) noexcept
{
    return 0.f;
}



/// <summary>
/// ホグラインのy座標を得る．
/// </summary>
/// <param name="side">false: シミュレーション座標系で負の側, true: シミュレーション座標系で正の側</param>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のホグラインのy座標</returns>
constexpr float GetHogLineY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kHogLineYOnSimulation : -detail::kHogLineYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



/// <summary>
/// ティーラインのy座標を得る．
/// </summary>
/// <param name="side">false: シミュレーション座標系で負の側, true: シミュレーション座標系で正の側</param>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のティーラインのy座標</returns>
constexpr float GetTeeLineY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kTeeLineYOnSimulation : -detail::kTeeLineYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



/// <summary>
/// バックラインのy座標を得る．
/// </summary>
/// <param name="side">false: シミュレーション座標系で負の側, true: シミュレーション座標系で正の側</param>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のバックラインのy座標</returns>
constexpr float GetBackLineY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kBackLineYOnSimulation : -detail::kBackLineYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



/// <summary>
/// ハックのy座標を得る．
/// </summary>
/// <param name="side">false: シミュレーション座標系で負の側, true: シミュレーション座標系で正の側</param>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のハックのy座標</returns>
constexpr float GetHackY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kHackYOnSimulation : -detail::kHackYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



/// <summary>
/// バックボード(シートの端)のy座標を得る．
/// </summary>
/// <param name="side">false: シミュレーション座標系で負の側, true: シミュレーション座標系で正の側</param>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のバックボードのy座標</returns>
constexpr float GetBackBoardY(bool side, Id coord) noexcept
{
    auto y_simulation = side ? detail::kBackBoardYOnSimulation : -detail::kBackBoardYOnSimulation;
    return TransformPosition(Vector2(0.f, y_simulation), Id::kSimulation, coord).y;
}



} // namespace digital_curling::coordinate

#endif // DIGITAL_CURLING_COORDINATE_HPP
