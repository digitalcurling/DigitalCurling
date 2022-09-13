/// \file
/// \brief ISimulatorStorage を定義します

#ifndef DIGITAL_CURLING_I_SIMULATOR_STORAGE_HPP
#define DIGITAL_CURLING_I_SIMULATOR_STORAGE_HPP

#include <memory>
#include <string>
#include "json/common.hpp"

namespace digital_curling {

class ISimulator;



/// \brief ISimulator の状態を保存するストレージ
///
/// ISimulator::Load() や ISimulatorStorage::CreateSimulator() を用いてストレージから状態を復元した場合，
/// ISimulator は ISimulator::Save() や ISimulator::CreateStorage() を用いてセーブした時点の内部状態を完全に復元します．
/// 言い換えると，あるストレージに保存された状態を復元した後のシミュレーションは，そのストレージに保存した時点からのシミュレーションと同じものになります．
///
/// ストレージは ISimulator のそれぞれの実装と対応関係にあるため， GetSimulatorId() で得られるIDが一致していない場合は保存/復元はできません．
class ISimulatorStorage {
public:
    ISimulatorStorage() = default;
    ISimulatorStorage(ISimulatorStorage const&) = delete;
    ISimulatorStorage & operator = (ISimulatorStorage const&) = delete;
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

} // namespace digital_curling


/// \cond Doxygen_Suppress
namespace digital_curling::polymorphic_json::detail {

template <>
ToJsonRegistry<ISimulatorStorage> & GetToJsonRegistry<ISimulatorStorage>();

template <>
FromJsonRegistry<ISimulatorStorage> & GetFromJsonRegistry<ISimulatorStorage>();

} // namespace digital_curling::polymorphic_json::detail
/// \endcond

/// \cond Doxygen_Suppress
namespace nlohmann {

template <>
struct adl_serializer<digital_curling::ISimulatorStorage> :
    digital_curling::polymorphic_json::detail::Serializer<digital_curling::ISimulatorStorage> {};

template <>
struct adl_serializer<std::unique_ptr<digital_curling::ISimulatorStorage>> :
    digital_curling::polymorphic_json::detail::Serializer<std::unique_ptr<digital_curling::ISimulatorStorage>> {};

} // namespace nlohmann
/// \endcond

#endif // DIGITAL_CURLING_I_SIMULATOR_STORAGE_HPP
