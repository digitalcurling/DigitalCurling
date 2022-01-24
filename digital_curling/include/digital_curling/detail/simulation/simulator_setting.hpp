#ifndef DIGITAL_CURLING_SIMULATION_SIMULATOR_SETTING_HPP
#define DIGITAL_CURLING_SIMULATION_SIMULATOR_SETTING_HPP

#include <memory>

namespace digital_curling::simulation {

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
};



} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SIMULATION_SIMULATOR_SETTING_HPP
