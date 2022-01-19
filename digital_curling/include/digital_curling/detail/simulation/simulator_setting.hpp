#ifndef DIGITAL_CURLING_SIMULATION_SIMULATOR_SETTING_HPP
#define DIGITAL_CURLING_SIMULATION_SIMULATOR_SETTING_HPP

#include <memory>
#include "../json.hpp"

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

    /// <summary>
    /// 実装用関数．JSONを出力する
    /// </summary>
    /// <param name="json">出力用JSON</param>
    virtual void ToJson(nlohmann::json & json) const = 0;
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


#endif // DIGITAL_CURLING_SIMULATION_SIMULATOR_SETTING_HPP
