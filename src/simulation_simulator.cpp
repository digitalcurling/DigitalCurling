#include "digital_curling/detail/simulation/simulator.hpp"
#include "simulation_fcv1.hpp"

namespace digital_curling::simulation {

void to_json(nlohmann::json & j, ISimulatorSetting const& setting)
{
    setting.ToJson(j);
}

} // namespace digital_curling::simulation



namespace nlohmann {

std::unique_ptr<digital_curling::simulation::ISimulatorSetting> adl_serializer<std::unique_ptr<digital_curling::simulation::ISimulatorSetting>>::from_json(json const & j)
{
    auto type = j.at("type").get<std::string>();

    if (type == digital_curling::simulation::SimulatorFCV1Setting::kType) {
        return std::make_unique<digital_curling::simulation::SimulatorFCV1Setting>(j.get<digital_curling::simulation::SimulatorFCV1Setting>());
    } else {
        throw std::runtime_error("no such type simulator.");
    }
}

void adl_serializer<std::unique_ptr<digital_curling::simulation::ISimulatorSetting>>::to_json(json & j, std::unique_ptr<digital_curling::simulation::ISimulatorSetting> const& setting)
{
    setting->ToJson(j);
}

} // namespace nlohmann
