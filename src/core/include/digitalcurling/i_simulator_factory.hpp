// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief ISimulatorFactory を定義

#pragma once

#include <memory>
#include <string>
#include "common.hpp"

namespace digitalcurling {

class ISimulator;

/// @brief ISimulator を構築するためのファクトリーインターフェース
///
/// `ISimulator` のインスタンスはこのクラスの派生クラスの `CreateSimulator()` で生成されます。
class ISimulatorFactory {
protected:
    ISimulatorFactory() = default;
    /// @brief コピーコンストラクタ
    ISimulatorFactory(ISimulatorFactory const&) = default;
    /// @brief コピー代入演算子
    ISimulatorFactory& operator = (ISimulatorFactory const&) = default;

public:
    virtual ~ISimulatorFactory() = default;

    /// @brief シミュレータを生成する
    /// @returns 生成されたシミュレータ
    virtual std::unique_ptr<ISimulator> CreateSimulator() const = 0;

    /// @brief このインスタンスを複製する
    /// @returns 複製されたインスタンス
    virtual std::unique_ptr<ISimulatorFactory> Clone() const = 0;

    /// @brief 対応するシミュレータのシミュレータIDを得る
    ///
    /// シミュレータIDはシミュレータの種類ごとに異なります。
    ///
    /// @returns シミュレータID
    virtual std::string GetSimulatorId() const = 0;
};

} // namespace digitalcurling
