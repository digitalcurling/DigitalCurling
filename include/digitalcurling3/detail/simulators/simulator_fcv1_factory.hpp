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

/// \file
/// \brief SimulatorFCV1Factory を定義します

#ifndef DIGITALCURLING3_SIMULATORS_SIMULATOR_FCV1_FACTORY_HPP
#define DIGITALCURLING3_SIMULATORS_SIMULATOR_FCV1_FACTORY_HPP

#include <string_view>
#include "../json/common.hpp"
#include "../i_simulator_factory.hpp"

namespace digitalcurling3::simulators {

/// \brief シミュレータFCV1のID
constexpr std::string_view kSimulatorFCV1Id = "fcv1";

/// \brief シミュレータFCV1(Friction-CurlVelocity 1)を構築するためのクラスです
///
/// 生成されたシミュレータFCV1は ISimulator インターフェイスを介してのみ扱うことができます
class SimulatorFCV1Factory : public ISimulatorFactory {
public:
    /// \brief フレームレート(フレーム毎秒)
    ///
    /// CreateSimulator() を呼び出す前にこの値を変更することで，
    /// 生成されるFCV1シミュレータのフレームレート( ISimulator::Step()
    /// の呼出しによる経過時間)を変更することができます．
    /// ただし，フレームレートをデフォルトの値から変更した際の動作の保証はしません．
    float seconds_per_frame = 0.001f;

    SimulatorFCV1Factory() = default;
    SimulatorFCV1Factory(SimulatorFCV1Factory const&) = default; ///< コピーコンストラクタ
    SimulatorFCV1Factory & operator = (SimulatorFCV1Factory const&) = default; ///< コピー代入演算子
    virtual ~SimulatorFCV1Factory() = default;

    virtual std::unique_ptr<ISimulator> CreateSimulator() const override;
    virtual std::unique_ptr<ISimulatorFactory> Clone() const override;
    virtual std::string GetSimulatorId() const override
    {
        return std::string(kSimulatorFCV1Id);
    }
};


/// \cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, SimulatorFCV1Factory const&);
void from_json(nlohmann::json const&, SimulatorFCV1Factory &);
/// \endcond

} // namespace digitalcurling3::simulators

#endif // DIGITALCURLING3_SIMULATORS_SIMULATOR_FCV1_FACTORY_HPP
