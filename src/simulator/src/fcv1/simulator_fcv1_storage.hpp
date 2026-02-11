// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief SimulatorFCV1Storage を定義

#pragma once

#include <memory>
#include <vector>
#include "digitalcurling/common.hpp"
#include "digitalcurling/simulators/i_simulator_storage.hpp"
#include "simulator_fcv1_factory.hpp"

namespace digitalcurling::simulators {

/// @brief シミュレータ FCV1 のストレージ
class SimulatorFCV1Storage : public ISimulatorStorage {
public:
    /// @brief デフォルトコンストラクタ
    SimulatorFCV1Storage() = default;
    /// @brief コピーコンストラクタ
    SimulatorFCV1Storage(SimulatorFCV1Storage const&) = default;
    /// @brief コピー代入演算子
    SimulatorFCV1Storage & operator = (SimulatorFCV1Storage const&) = default;
    /// @brief コンストラクタ
    /// @param factory ストレージを初期化するためのファクトリー
    SimulatorFCV1Storage(SimulatorFCV1Factory const& factory);
    virtual ~SimulatorFCV1Storage() override = default;

    virtual const char* GetId() const noexcept override { return DIGITALCURLING_PLUGIN_NAME; }
    virtual nlohmann::json ToJson() const override;

    virtual std::unique_ptr<ISimulator> CreateSimulator() const override;

    /// @brief このストレージに保存されたシミュレータのファクトリー情報
    SimulatorFCV1Factory factory;
    /// @brief 全ストーンの位置と速度
    ISimulator::AllStones stones;
    /// @brief 衝突情報
    std::vector<ISimulator::Collision> collisions;
};


/// @cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, SimulatorFCV1Storage const&);
void from_json(nlohmann::json const&, SimulatorFCV1Storage &);
/// @endcond

} // namespace digitalcurling::simulators
