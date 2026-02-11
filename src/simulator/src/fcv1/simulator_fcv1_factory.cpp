// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "digitalcurling/detail/simulators/simulator_fcv1_factory.hpp"
#include "simulators_simulator_fcv1.hpp"

namespace digitalcurling::simulators {

std::unique_ptr<ISimulator> SimulatorFCV1Factory::CreateSimulator() const
{
    return std::make_unique<SimulatorFCV1>(*this);
}

std::unique_ptr<ISimulatorFactory> SimulatorFCV1Factory::Clone() const
{
    return std::make_unique<SimulatorFCV1Factory>(*this);
}


// json
void to_json(nlohmann::json & j, SimulatorFCV1Factory const& v)
{
    j["type"] = kSimulatorFCV1Id;
    j["seconds_per_frame"] = v.seconds_per_frame;
}

void from_json(nlohmann::json const& j, SimulatorFCV1Factory & v)
{
    j.at("seconds_per_frame").get_to(v.seconds_per_frame);
}

} // namespace digitalcurling::simulators
