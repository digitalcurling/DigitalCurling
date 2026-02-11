#include <gtest/gtest.h>
#include "digitalcurling/digitalcurling.hpp"

namespace dc = digitalcurling;
namespace dcs = digitalcurling::simulators;
using nlohmann::json;

namespace digitalcurling::test {

bool EqualsSimulatorStones(dcs::ISimulator::AllStones const& stones0, dcs::ISimulator::AllStones const& stones1);

} // unnamed namespace
