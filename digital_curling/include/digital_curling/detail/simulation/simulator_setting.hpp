/// \file
/// \brief ISimulatorSetting を定義します

#ifndef DIGITAL_CURLING_SIMULATION_SIMULATOR_SETTING_HPP
#define DIGITAL_CURLING_SIMULATION_SIMULATOR_SETTING_HPP

#include <memory>

namespace digital_curling::simulation {

class ISimulator;



/// \brief ISimulator を構築するためのクラスです
///
/// ISimulator のインスタンスはこのクラスの派生クラスの CreateSimulator() で生成されます．
class ISimulatorSetting {
public:
    virtual ~ISimulatorSetting() = default;

    /// \brief シミュレータを生成する
    ///
    /// \returns 生成されたシミュレータ
    virtual std::unique_ptr<ISimulator> CreateSimulator() const = 0;
};



} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SIMULATION_SIMULATOR_SETTING_HPP
