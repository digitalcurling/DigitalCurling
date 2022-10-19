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

#ifndef DIGITALCURLING3_SRC_SIMULATORS_FCV1_SIMULATOR_STORAGE_HPP
#define DIGITALCURLING3_SRC_SIMULATORS_FCV1_SIMULATOR_STORAGE_HPP

#include "digitalcurling3/detail/i_simulator.hpp"
#include "digitalcurling3/detail/i_simulator_storage.hpp"
#include "digitalcurling3/detail/simulators/fcv1_simulator_factory.hpp"

namespace digitalcurling3::simulators {

/// \brief FCV1シミュレータ用ストレージ
class FCV1SimulatorStorage : public ISimulatorStorage {
public:
    FCV1SimulatorStorage() = default;
    FCV1SimulatorStorage(FCV1SimulatorStorage const&) = default;
    FCV1SimulatorStorage & operator = (FCV1SimulatorStorage const&) = default;
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

} // namespace digitalcurling3::simulators

#endif // DIGITALCURLING3_SRC_SIMULATORS_FCV1_SIMULATOR_STORAGE_HPP
