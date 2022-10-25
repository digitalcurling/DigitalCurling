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

#include "digitalcurling3/detail/simulators/simulator_fcv1_factory.hpp"
#include "simulators_simulator_fcv1.hpp"

namespace digitalcurling3::simulators {

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

} // namespace digitalcurling3::simulators
