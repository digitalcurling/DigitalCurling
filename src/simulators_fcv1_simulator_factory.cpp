// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "digitalcurling3/detail/simulators/fcv1_simulator_factory.hpp"
#include "simulators_fcv1_simulator.hpp"

namespace digitalcurling3::simulators {

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

} // namespace digitalcurling3::simulators
