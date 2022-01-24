/// \file
/// \brief SimulatorFCV1Setting を定義します

#ifndef DIGITAL_CURLING_SIMULATION_SIMULATOR_FCV1_SETTING_HPP
#define DIGITAL_CURLING_SIMULATION_SIMULATOR_FCV1_SETTING_HPP

#include <memory>
#include "simulator_setting.hpp"

namespace digital_curling::simulation {



/// \brief FCV1(Friction-CurlVelocity 1)シミュレータを構築するためのクラスです
///
/// 生成されたFCV1シミュレータは ISimulator インターフェイスを介してのみ扱うことができます
class SimulatorFCV1Setting : public ISimulatorSetting {
public:
    /// \brief フレームレート(フレーム毎秒)
    ///
    /// CreateSimulator() を呼び出す前にこの値を変更することで，
    /// 生成されるFCV1シミュレータのフレームレート( ISimulator::Step()
    /// の呼出しによる経過時間)を変更することができます．
    /// ただし，フレームレートをデフォルトの値から変更した際の動作保証はしません．
    float seconds_per_frame = 0.001f;

    SimulatorFCV1Setting() = default;
    virtual ~SimulatorFCV1Setting() = default;

    virtual std::unique_ptr<ISimulator> CreateSimulator() const override;
};



} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SIMULATION_SIMULATOR_FCV1_SETTING_HPP
