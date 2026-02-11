// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief SimulatorFCV1Factory を定義します

#pragma once

#include <string_view>
#include "../json/common.hpp"
#include "../i_simulator_factory.hpp"

namespace digitalcurling::simulators {

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

} // namespace digitalcurling::simulators
