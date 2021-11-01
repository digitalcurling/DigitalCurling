#ifndef DIGITAL_CURLING_SIMULATION_SIMULATOR_HPP
#define DIGITAL_CURLING_SIMULATION_SIMULATOR_HPP

#include <vector>
#include <memory>
#include <string>
#include "../common.hpp"

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



/// <summary>
/// ストーンどうしの衝突の情報
/// </summary>
struct StoneCollision {

    /// <summary>
    /// ストーンAのID
    /// </summary>
    StoneId a_id;

    /// <summary>
    /// ストーンBのID
    /// </summary>
    StoneId b_id;

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

    StoneCollision(StoneId a_id, StoneId b_id, Vector2 a_position, Vector2 b_position, float normal_impulse, float tangent_impulse)
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



class ISimulator;

/// <summary>
/// <see cref="ISimulator"/>を構築するための設定
/// </summary>
class ISimulatorSetting {
public:
    virtual ~ISimulatorSetting() = default;

    /// <summary>
    /// シミュレータを作成する
    /// </summary>
    /// <returns>新たに作成されたシミュレータ</returns>
    virtual std::unique_ptr<ISimulator> CreateSimulator() const = 0;

    /// <summary>
    /// 実装用関数．JSONを出力する
    /// </summary>
    /// <param name="json">出力用JSON</param>
    virtual void ToJson(nlohmann::json & json) const = 0;
};




/// <summary>
/// ストーンの挙動を再現する物理シミュレータ
/// </summary>
class ISimulator {
public:
    /// <summary>
    /// ストーンの情報を設定する．
    /// </summary>
    /// <param name="stones">すべてのストーンの情報</param>
    virtual void SetStones(AllStoneData const& stones) = 0;

    /// <summary>
    /// 1フレーム進める．
    /// </summary>
    virtual void Step() = 0;

    /// <summary>
    /// ストーンの情報を取得する．
    /// </summary>
    /// <returns>すべてのストーンの情報</returns>
    virtual AllStoneData const& GetStones() const = 0;

    /// <summary>
    /// 前回の<see cref="Step"/>の呼出しで発生したすべての衝突の情報を得る．
    /// </summary>
    /// <returns></returns>
    virtual std::vector<StoneCollision> const& GetCollisions() const = 0;

    /// <summary>
    /// ストーンが停止しているかをチェックする．
    /// </summary>
    /// <returns>すべてのストーンが停止していれば<c>true</c>を返す．</returns>
    virtual bool AreAllStonesStopped() const = 0;

    /// <summary>
    /// 1回の<see cref="Step"/>の呼出しで進む時間を得る．
    /// </summary>
    /// <returns>1回の<see cref="Step"/>の呼出しで進む時間(秒)</returns>
    virtual float GetSecondsPerFrame() const = 0;

    /// <summary>
    /// ストーンの半径を得る．
    /// </summary>
    /// <returns>ストーンの半径</returns>
    virtual float GetStoneRadius() const = 0;

    /// <summary>
    /// このシミュレータ構築時の設定を得る．
    /// </summary>
    /// <returns>設定情報</returns>
    virtual ISimulatorSetting const& GetSetting() const = 0;

    ISimulator() = default;
    ISimulator(ISimulator const&) = delete;
    ISimulator& operator = (ISimulator const&) = delete;
    virtual ~ISimulator() = default;
};

} // namespace digital_curling::simulation



// json

namespace digital_curling::simulation {

void to_json(nlohmann::json &, ISimulatorSetting const&);

} // namespace digital_curling::simulation

namespace nlohmann {

template <>
struct adl_serializer<std::unique_ptr<digital_curling::simulation::ISimulatorSetting>> {
    static std::unique_ptr<digital_curling::simulation::ISimulatorSetting> from_json(json const&);
    static void to_json(json &, std::unique_ptr<digital_curling::simulation::ISimulatorSetting> const&);
};

} // namespace nlohmann

#endif // DIGITAL_CURLING_SIMULATION_SIMULATOR_HPP
