// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#pragma once

#include "digitalcurling3/detail/i_simulator.hpp"
#include "digitalcurling3/detail/i_simulator_storage.hpp"
#include "digitalcurling3/detail/simulators/simulator_fcv1_factory.hpp"

namespace digitalcurling3::simulators {

/// \brief FCV1シミュレータ用ストレージ
class SimulatorFCV1Storage : public ISimulatorStorage {
public:
    SimulatorFCV1Storage() = default;
    SimulatorFCV1Storage(SimulatorFCV1Storage const&) = default;
    SimulatorFCV1Storage & operator = (SimulatorFCV1Storage const&) = default;
    SimulatorFCV1Storage(SimulatorFCV1Factory const& factory);

    virtual ~SimulatorFCV1Storage() = default;

    virtual std::unique_ptr<ISimulator> CreateSimulator() const override;
    virtual std::string GetSimulatorId() const override
    {
        return std::string(kSimulatorFCV1Id);
    }

    SimulatorFCV1Factory factory;
    ISimulator::AllStones stones;
    std::vector<ISimulator::Collision> collisions;
};


// json
void to_json(nlohmann::json &, SimulatorFCV1Storage const&);
void from_json(nlohmann::json const&, SimulatorFCV1Storage &);

} // namespace digitalcurling3::simulators
