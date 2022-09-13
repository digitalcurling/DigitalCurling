/// \file
/// \brief ISimulatorFactory を定義します

#ifndef DIGITAL_CURLING_I_SIMULATOR_FACTORY_HPP
#define DIGITAL_CURLING_I_SIMULATOR_FACTORY_HPP

#include <memory>
#include <string>
#include "json/common.hpp"

namespace digital_curling {

class ISimulator;



/// \brief ISimulator を構築するためのクラスです
///
/// ISimulator のインスタンスはこのクラスの派生クラスの CreateSimulator() で生成されます．
class ISimulatorFactory {
protected:
    ISimulatorFactory() = default;
    ISimulatorFactory(ISimulatorFactory const&) = default;
    ISimulatorFactory& operator = (ISimulatorFactory const&) = default;

public:
    virtual ~ISimulatorFactory() = default;

    /// \brief シミュレータを生成する
    ///
    /// \returns 生成されたシミュレータ
    virtual std::unique_ptr<ISimulator> CreateSimulator() const = 0;

    /// \brief このインスタンスを複製する
    ///
    /// \returns 複製されたインスタンス
    virtual std::unique_ptr<ISimulatorFactory> Clone() const = 0;

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
ToJsonRegistry<ISimulatorFactory> & GetToJsonRegistry<ISimulatorFactory>();

template <>
FromJsonRegistry<ISimulatorFactory> & GetFromJsonRegistry<ISimulatorFactory>();

} // namespace digital_curling::polymorphic_json::detail
/// \endcond



/// \cond Doxygen_Suppress
namespace nlohmann {

template <>
struct adl_serializer<digital_curling::ISimulatorFactory> :
    digital_curling::polymorphic_json::detail::Serializer<digital_curling::ISimulatorFactory> {};

template <>
struct adl_serializer<std::unique_ptr<digital_curling::ISimulatorFactory>> :
    digital_curling::polymorphic_json::detail::Serializer<std::unique_ptr<digital_curling::ISimulatorFactory>> {};

} // namespace nlohmann
/// \endcond

#endif // DIGITAL_CURLING_I_SIMULATOR_FACTORY_HPP
