#ifndef DIGITAL_CURLING_COMMON_HPP
#define DIGITAL_CURLING_COMMON_HPP

#include <cstdint>
#include <cmath>
#include <array>
#include <optional>
#include "nlohmann/json.hpp"

namespace digital_curling {



/// <summary>
/// 円周率
/// </summary>
constexpr float kPi = 3.14159265359f;



/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 {
    float x;
    float y;

    constexpr Vector2() : x(0.f), y(0.f) {}
    constexpr Vector2(float x, float y) : x(x), y(y) {}

    constexpr Vector2 & operator += (Vector2 v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    constexpr Vector2 & operator -= (Vector2 v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    constexpr Vector2 & operator *= (float f)
    {
        x *= f;
        y *= f;
        return *this;
    }

    constexpr Vector2 & operator /= (float f)
    {
        x /= f;
        y /= f;
        return *this;
    }

    float Length() const
    {
        return std::hypot(x, y);
    }
};

// 単項
constexpr Vector2 operator - (Vector2 v)
{
    return { -v.x, -v.y };
}

constexpr Vector2 operator + (Vector2 v1, Vector2 v2)
{
    return { v1.x + v2.x, v1.y + v2.y };
}

constexpr Vector2 operator - (Vector2 v1, Vector2 v2)
{
    return { v1.x - v2.x, v1.y - v2.y };
}

constexpr Vector2 operator * (float f, Vector2 v)
{
    return { f * v.x, f * v.y };
}

constexpr Vector2 operator * (Vector2 v, float f)
{
    return f * v;
}

constexpr Vector2 operator / (Vector2 v, float f)
{
    return { v.x / f, v.y / f };
}

void to_json(nlohmann::json &, Vector2 const&);
void from_json(nlohmann::json const&, Vector2 &);



/// <summary>
/// ストーンのインデックスに用いる型
/// </summary>
using StoneId = std::uint8_t;



/// <summary>
/// シミュレータ上に配置可能なストーンの最大数
/// </summary>
constexpr StoneId kStoneMax = 16;



namespace coordinate {

namespace detail {

constexpr float kHogLineYOnSimulation = 10.9725f;
constexpr float kTeeLineYOnSimulation = 17.3735f;
constexpr float kBackLineYOnSimulation = 19.2025f;
constexpr float kHackYOnSimulation = 21.0315f;
constexpr float kBackBoardYOnSimulation = 22.8605f;

} // namespace detail



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

    // 軸の方向が変化するのはどちらかがKShot1の場合のみ
    if (from == Id::kShot1 || to == Id::kShot1) {
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
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のホグラインのy座標</returns>
constexpr float GetHogLineY(Id coord) noexcept
{
    return TransformPosition(Vector2(0.f, detail::kHogLineYOnSimulation), Id::kSimulation, coord).y;
}



/// <summary>
/// ティーラインのy座標を得る．
/// </summary>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のティーラインのy座標</returns>
constexpr float GetTeeLineY(Id coord) noexcept
{
    return TransformPosition(Vector2(0.f, detail::kTeeLineYOnSimulation), Id::kSimulation, coord).y;
}



/// <summary>
/// バックラインのy座標を得る．
/// </summary>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のバックラインのy座標</returns>
constexpr float GetBackLineY(Id coord) noexcept
{
    return TransformPosition(Vector2(0.f, detail::kBackLineYOnSimulation), Id::kSimulation, coord).y;
}



/// <summary>
/// ハックのy座標を得る．
/// </summary>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のハックのy座標</returns>
constexpr float GetHackY(Id coord) noexcept
{
    return TransformPosition(Vector2(0.f, detail::kHackYOnSimulation), Id::kSimulation, coord).y;
}



/// <summary>
/// バックボード(シートの端)のy座標を得る．
/// </summary>
/// <param name="coord">座標系</param>
/// <returns><paramref name="coord"/>座標系のバックボードのy座標</returns>
constexpr float GetBackBoardY(Id coord) noexcept
{
    return TransformPosition(Vector2(0.f, detail::kBackBoardYOnSimulation), Id::kSimulation, coord).y;
}



constexpr float kHouseRadius = 1.829f;

} // namespace coordinate

} // namespace digital_curling



namespace nlohmann {

// For serializing/deserializing std::optional to/from json
template <typename T>
struct adl_serializer<std::optional<T>> {
    static void to_json(json & j, std::optional<T> const& opt)
    {
        if (opt) {
            j = *opt;
        } else {
            j = nullptr;
        }
    }

    static void from_json(json const& j, std::optional<T> & opt)
    {
        if (j.is_null()) {
            opt = std::nullopt;
        } else {
            opt = j.get<T>();
        }
    }
};

} // namespace nlohmann

#endif // DIGITAL_CURLING_COMMON_HPP
