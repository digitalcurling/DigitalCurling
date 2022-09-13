#include "simulators_fcv1_simulator_storage.hpp"

#include "simulators_fcv1_simulator.hpp"

namespace digital_curling::simulators {

FCV1SimulatorStorage::FCV1SimulatorStorage(FCV1SimulatorFactory const& factory)
    : factory(factory)
    , stones()
    , collisions()
{}

std::unique_ptr<ISimulator> FCV1SimulatorStorage::CreateSimulator() const
{
    return std::make_unique<FCV1Simulator>(*this);
}

// json
void to_json(nlohmann::json & j, FCV1SimulatorStorage const& v)
{
    j["type"] = kFCV1SimulatorId;
    j["factory"] = v.factory;
    j["stones"] = v.stones;
    j["collisions"] = v.collisions;
}

void from_json(nlohmann::json const& j, FCV1SimulatorStorage & v)
{
    j.at("factory").get_to(v.factory);
    j.at("stones").get_to(v.stones);
    j.at("collisions").get_to(v.collisions);
}

} // namespace digital_curling::simulators
