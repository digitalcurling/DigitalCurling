#include "digital_curling/detail/i_simulator_storage.hpp"
#include "simulators_fcv1_simulator_storage.hpp"

namespace digital_curling::polymorphic_json::detail {

template <>
ToJsonRegistry<ISimulatorStorage> & GetToJsonRegistry<ISimulatorStorage>()
{
    static ToJsonRegistry<ISimulatorStorage> registry{
        { typeid(simulators::FCV1SimulatorStorage), ToJsonFuncTemplate<ISimulatorStorage, simulators::FCV1SimulatorStorage> },
    };
    return registry;
}

template <>
FromJsonRegistry<ISimulatorStorage> & GetFromJsonRegistry<ISimulatorStorage>()
{
    static FromJsonRegistry<ISimulatorStorage> registry{
        { std::string(simulators::kFCV1SimulatorId), FromJsonFuncTemplate<ISimulatorStorage, simulators::FCV1SimulatorStorage> },
    };
    return registry;
}

} // namespace digital_curling::polymorphic_json::detail
