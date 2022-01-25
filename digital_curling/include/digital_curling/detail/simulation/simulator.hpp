/// \file
/// \brief ISimulator を定義します

#ifndef DIGITAL_CURLING_SIMULATION_SIMULATOR_HPP
#define DIGITAL_CURLING_SIMULATION_SIMULATOR_HPP

#include <vector>
#include "../vector2.hpp"
#include "stone_data.hpp"
#include "stone_collision.hpp"

namespace digital_curling::simulation {

class ISimulatorSetting;



/// \brief ストーンの挙動を再現する物理シミュレータを扱うためのインターフェイス
///
/// ISimulator のインスタンスは ISimulatorSetting の派生クラスの
/// ISimulatorSetting::CreateSimulator() でのみ生成されます．
///
/// このシミュレータではカーリングのルールやアイスシートの大きさ制限は記述されません．
/// それらについては ApplyMove() で記述されます．
///
/// \sa ISimulatorSetting, ApplyMove()
class ISimulator {
public:
    /// \brief 全ストーンの情報を設定する．
    ///
    /// \param[in] stones 全ストーンの情報(位置，角度，速度，角速度)
    virtual void SetStones(AllStoneData const& stones) = 0;

    /// \brief 1フレーム進める．
    ///
    /// 1回呼び出すと GetSecondsPerFrame() の返り値の分だけシミュレータ内部の時間が進みます
    virtual void Step() = 0;

    /// \brief 全ストーンの情報を取得する．
    ///
    /// \returns 全ストーンの情報(位置，角度，速度，角速度)
    virtual AllStoneData const& GetStones() const = 0;

    /// \brief 直前の Step() の呼出しで発生したすべての衝突の情報を得る．
    ///
    /// \warning この関数の返り値は内部の std::vector のインスタンスへの参照ですが，
    ///          この参照は次の Step() の呼出しで無効になり得ます．
    ///          衝突のデータを保持する場合，次の Step() を呼び出す前にデータをコピーして下さい．
    /// \returns 直前の Step() の呼出しで発生したすべての衝突
    virtual std::vector<StoneCollision> const& GetCollisions() const = 0;

    /// \brief 全ストーンが停止しているかをチェックする．
    ///
    /// \returns 全ストーンが停止していれば \c true ，一つでも停止していないストーンがあれば \c false
    virtual bool AreAllStonesStopped() const = 0;

    /// \brief 1回 Step() の呼出しで進む時間を得る．
    ///
    /// \returns 1回の Step() の呼出しで進む時間(秒)
    virtual float GetSecondsPerFrame() const = 0;

    /// \brief ストーンの半径(m)を得る．
    ///
    /// \returns ストーンの半径(m)
    virtual float GetStoneRadius() const = 0;

    /// \brief このシミュレータを構築した際の設定を得る．
    ///
    /// \returns このシミュレータを構築した際の設定
    virtual ISimulatorSetting const& GetSetting() const = 0;

    ISimulator() = default;
    ISimulator(ISimulator const&) = delete;
    ISimulator& operator = (ISimulator const&) = delete;
    virtual ~ISimulator() = default;
};



} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SIMULATION_SIMULATOR_HPP
