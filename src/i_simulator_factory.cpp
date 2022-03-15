#include "digital_curling/detail/i_simulator_factory.hpp"
#include "digital_curling/detail/simulators/fcv1_simulator_factory.hpp"

namespace digital_curling::polymorphic_json::detail {

template <>
ToJsonRegistry<ISimulatorFactory> & GetToJsonRegistry<ISimulatorFactory>()
{
    static ToJsonRegistry<ISimulatorFactory> registry{
        { typeid(simulators::FCV1SimulatorFactory), ToJsonFuncTemplate<ISimulatorFactory, simulators::FCV1SimulatorFactory> },
    };
    return registry;
}

template <>
FromJsonRegistry<ISimulatorFactory> & GetFromJsonRegistry<ISimulatorFactory>()
{
    static FromJsonRegistry<ISimulatorFactory> registry{
        { std::string(simulators::kFCV1SimulatorId), FromJsonFuncTemplate<ISimulatorFactory, simulators::FCV1SimulatorFactory> },
    };
    return registry;
}

} // namespace digital_curling::polymorphic_json::detail
