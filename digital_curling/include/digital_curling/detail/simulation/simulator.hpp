#ifndef DIGITAL_CURLING_SIMULATION_SIMULATOR_HPP
#define DIGITAL_CURLING_SIMULATION_SIMULATOR_HPP

#include <vector>
#include "../vector2.hpp"
#include "stone_data.hpp"
#include "stone_collision.hpp"

namespace digital_curling::simulation {

class ISimulatorSetting;



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

#endif // DIGITAL_CURLING_SIMULATION_SIMULATOR_HPP
