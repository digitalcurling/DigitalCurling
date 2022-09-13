#include "digital_curling/detail/simulators/fcv1_simulator_factory.hpp"
#include "simulators_fcv1_simulator.hpp"

namespace digital_curling::simulators {

FCV1SimulatorFactory::FCV1SimulatorFactory(FCV1SimulatorFactory const& rhs)
    : seconds_per_frame(rhs.seconds_per_frame)
{}

FCV1SimulatorFactory & FCV1SimulatorFactory::operator = (FCV1SimulatorFactory const& rhs)
{
    if (this == &rhs) return *this;

    seconds_per_frame = rhs.seconds_per_frame;

    return *this;
}

std::unique_ptr<ISimulator> FCV1SimulatorFactory::CreateSimulator() const
{
    return std::make_unique<FCV1Simulator>(*this);
}

std::unique_ptr<ISimulatorFactory> FCV1SimulatorFactory::Clone() const
{
    return std::make_unique<FCV1SimulatorFactory>(*this);
}


// json
void to_json(nlohmann::json & j, FCV1SimulatorFactory const& v)
{
    j["type"] = kFCV1SimulatorId;
    j["seconds_per_frame"] = v.seconds_per_frame;
}

void from_json(nlohmann::json const& j, FCV1SimulatorFactory & v)
{
    j.at("seconds_per_frame").get_to(v.seconds_per_frame);
}

} // namespace digital_curling::simulators
