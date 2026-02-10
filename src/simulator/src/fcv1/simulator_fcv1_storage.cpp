// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "simulators_simulator_fcv1_storage.hpp"

#include "simulators_simulator_fcv1.hpp"

namespace digitalcurling3::simulators {

SimulatorFCV1Storage::SimulatorFCV1Storage(SimulatorFCV1Factory const& factory)
    : factory(factory)
    , stones()
    , collisions()
{}

std::unique_ptr<ISimulator> SimulatorFCV1Storage::CreateSimulator() const
{
    return std::make_unique<SimulatorFCV1>(*this);
}

// json
void to_json(nlohmann::json & j, SimulatorFCV1Storage const& v)
{
    j["type"] = kSimulatorFCV1Id;
    j["factory"] = v.factory;
    j["stones"] = v.stones;
    j["collisions"] = v.collisions;
}

void from_json(nlohmann::json const& j, SimulatorFCV1Storage & v)
{
    j.at("factory").get_to(v.factory);
    j.at("stones").get_to(v.stones);
    j.at("collisions").get_to(v.collisions);
}

} // namespace digitalcurling3::simulators
