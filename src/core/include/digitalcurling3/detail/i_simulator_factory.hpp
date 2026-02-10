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
/// \brief ISimulatorFactory を定義します

#ifndef DIGITALCURLING3_I_SIMULATOR_FACTORY_HPP
#define DIGITALCURLING3_I_SIMULATOR_FACTORY_HPP

#include <memory>
#include <string>
#include "json/common.hpp"

namespace digitalcurling3 {

class ISimulator;



/// \brief ISimulator を構築するためのクラスです
///
/// ISimulator のインスタンスはこのクラスの派生クラスの CreateSimulator() で生成されます．
class ISimulatorFactory {
protected:
    ISimulatorFactory() = default;
    ISimulatorFactory(ISimulatorFactory const&) = default; ///< コピーコンストラクタ．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．
    ISimulatorFactory& operator = (ISimulatorFactory const&) = default; ///< コピー代入演算子．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．

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

} // namespace digitalcurling3



/// \cond Doxygen_Suppress
namespace digitalcurling3::polymorphic_json::detail {

template <>
ToJsonRegistry<ISimulatorFactory> & GetToJsonRegistry<ISimulatorFactory>();

template <>
FromJsonRegistry<ISimulatorFactory> & GetFromJsonRegistry<ISimulatorFactory>();

} // namespace digitalcurling3::polymorphic_json::detail
/// \endcond



/// \cond Doxygen_Suppress
namespace nlohmann {

template <>
struct adl_serializer<digitalcurling3::ISimulatorFactory> :
    digitalcurling3::polymorphic_json::detail::Serializer<digitalcurling3::ISimulatorFactory> {};

template <>
struct adl_serializer<std::unique_ptr<digitalcurling3::ISimulatorFactory>> :
    digitalcurling3::polymorphic_json::detail::Serializer<std::unique_ptr<digitalcurling3::ISimulatorFactory>> {};

} // namespace nlohmann
/// \endcond

#endif // DIGITALCURLING3_I_SIMULATOR_FACTORY_HPP
