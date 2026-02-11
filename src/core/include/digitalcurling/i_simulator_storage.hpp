// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief ISimulatorStorage を定義

#pragma once

#include <memory>
#include <string>
#include "digitalcurling/common.hpp"

namespace digitalcurling {

class ISimulator;

/// @brief ISimulator の状態を保存するストレージインターフェース
///
/// `ISimulator::Load()` や `ISimulatorStorage::CreateSimulator()` を用いてストレージから状態を復元した場合、
/// `ISimulator` は `ISimulator::Save()` や `ISimulator::CreateStorage()` を用いてセーブした時点の内部状態を完全に復元します。
/// すなわち、あるストレージに保存された状態を復元した後のシミュレーションは、そのストレージに保存した時点からのシミュレーションと同じものになります。
///
/// ストレージは `ISimulator` のそれぞれの実装と対応関係にあるため、 `GetSimulatorId()` で得られるIDが一致していない場合は保存/復元はできません。
class ISimulatorStorage {
protected:
    ISimulatorStorage() = default;
    /// @brief コピーコンストラクタ
    ISimulatorStorage(ISimulatorStorage const&) = default;
    /// @brief コピー代入演算子
    ISimulatorStorage & operator = (ISimulatorStorage const&) = default;

public:
    virtual ~ISimulatorStorage() = default;

    /// @brief このストレージが持つ状態と同じ状態を持つシミュレータを生成する
    /// @returns 生成されたシミュレータ
    virtual std::unique_ptr<ISimulator> CreateSimulator() const = 0;

    /// @brief 対応するシミュレータのシミュレータIDを得る
    ///
    /// シミュレータIDはシミュレータの種類ごとに異なります。
    ///
    /// @returns シミュレータID
    virtual std::string GetSimulatorId() const = 0;
};

} // namespace digitalcurling
