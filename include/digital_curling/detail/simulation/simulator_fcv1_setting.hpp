#ifndef DIGITAL_CURLING_SIMULATION_SIMULATOR_FCV1_SETTING_HPP
#define DIGITAL_CURLING_SIMULATION_SIMULATOR_FCV1_SETTING_HPP

#include <memory>
#include "../json.hpp"
#include "simulator_setting.hpp"

namespace digital_curling::simulation {



/// <summary>
/// FCV1(Friction-CurlVelocity 1)シミュレータの設定
/// </summary>
class SimulatorFCV1Setting : public ISimulatorSetting {
public:
    static constexpr char kType[] = "fcv1";

    /// <summary>
    /// フレームレート(フレーム毎秒)
    /// </summary>
    float seconds_per_frame = 0.001f;

    SimulatorFCV1Setting() = default;
    virtual ~SimulatorFCV1Setting() = default;

    virtual std::unique_ptr<ISimulator> CreateSimulator() const override;
    virtual void ToJson(nlohmann::json & json) const override;
};



} // namespace digital_curling::simulation



// json

namespace digital_curling::simulation {

void to_json(nlohmann::json &, SimulatorFCV1Setting const&);
void from_json(nlohmann::json const&, SimulatorFCV1Setting &);

} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SIMULATION_SIMULATOR_FCV1_SETTING_HPP
