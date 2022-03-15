#ifndef DIGITAL_CURLING_SRC_SIMULATORS_FCV1_SIMULATOR_STORAGE_HPP
#define DIGITAL_CURLING_SRC_SIMULATORS_FCV1_SIMULATOR_STORAGE_HPP

#include "digital_curling/detail/i_simulator.hpp"
#include "digital_curling/detail/i_simulator_storage.hpp"
#include "digital_curling/detail/simulators/fcv1_simulator_factory.hpp"

namespace digital_curling::simulators {

/// <summary>
/// FCV1シミュレータ用ストレージ
/// </summary>
class FCV1SimulatorStorage : public ISimulatorStorage {
public:
    FCV1SimulatorStorage() = default;
    FCV1SimulatorStorage(FCV1SimulatorStorage const&);
    FCV1SimulatorStorage & operator = (FCV1SimulatorStorage const&);
    FCV1SimulatorStorage(FCV1SimulatorFactory const& factory);

    virtual ~FCV1SimulatorStorage() = default;

    virtual std::unique_ptr<ISimulator> CreateSimulator() const override;
    virtual std::string GetSimulatorId() const override
    {
        return std::string(kFCV1SimulatorId);
    }

    FCV1SimulatorFactory factory;
    ISimulator::AllStones stones;
    std::vector<ISimulator::Collision> collisions;
};


// json
void to_json(nlohmann::json &, FCV1SimulatorStorage const&);
void from_json(nlohmann::json const&, FCV1SimulatorStorage &);

} // namespace digital_curling::simulators

#endif // DIGITAL_CURLING_SRC_SIMULATORS_FCV1_SIMULATOR_STORAGE_HPP
