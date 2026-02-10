// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief ISimulatorStorage を定義します

#ifndef DIGITALCURLING3_I_SIMULATOR_STORAGE_HPP
#define DIGITALCURLING3_I_SIMULATOR_STORAGE_HPP

#include <memory>
#include <string>
#include "common.hpp"

namespace digitalcurling3 {

class ISimulator;



/// \brief ISimulator の状態を保存するストレージ
///
/// ISimulator::Load() や ISimulatorStorage::CreateSimulator() を用いてストレージから状態を復元した場合，
/// ISimulator は ISimulator::Save() や ISimulator::CreateStorage() を用いてセーブした時点の内部状態を完全に復元します．
/// 言い換えると，あるストレージに保存された状態を復元した後のシミュレーションは，そのストレージに保存した時点からのシミュレーションと同じものになります．
///
/// ストレージは ISimulator のそれぞれの実装と対応関係にあるため， GetSimulatorId() で得られるIDが一致していない場合は保存/復元はできません．
class ISimulatorStorage {
protected:
    ISimulatorStorage() = default;
    ISimulatorStorage(ISimulatorStorage const&) = default; ///< コピーコンストラクタ．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．
    ISimulatorStorage & operator = (ISimulatorStorage const&) = default; ///< コピー代入演算子．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．

public:
    virtual ~ISimulatorStorage() = default;

    /// \brief このストレージが持つ状態と同じ状態を持つシミュレータを生成する
    ///
    /// \returns 生成されたシミュレータ
    virtual std::unique_ptr<ISimulator> CreateSimulator() const = 0;

    /// \brief 対応するシミュレータのシミュレータIDを得る．
    ///
    /// シミュレータIDはシミュレータの種類ごとに異なる．
    ///
    /// \returns シミュレータID
    virtual std::string GetSimulatorId() const = 0;
};

} // namespace digitalcurling3

#endif // DIGITALCURLING3_I_SIMULATOR_STORAGE_HPP
