/// \file
/// \brief FCV1SimulatorFactory を定義します

#ifndef DIGITAL_CURLING_SIMULATORS_FCV1_SIMULATOR_FACTORY_HPP
#define DIGITAL_CURLING_SIMULATORS_FCV1_SIMULATOR_FACTORY_HPP

#include <string_view>
#include "../json/common.hpp"
#include "../i_simulator_factory.hpp"

namespace digital_curling::simulators {

/// \brief FCV1シミュレータのID
constexpr std::string_view kFCV1SimulatorId = "fcv1";

/// \brief FCV1(Friction-CurlVelocity 1)シミュレータを構築するためのクラスです
///
/// 生成されたFCV1シミュレータは ISimulator インターフェイスを介してのみ扱うことができます
class FCV1SimulatorFactory : public ISimulatorFactory {
public:
    /// \brief フレームレート(フレーム毎秒)
    ///
    /// CreateSimulator() を呼び出す前にこの値を変更することで，
    /// 生成されるFCV1シミュレータのフレームレート( ISimulator::Step()
    /// の呼出しによる経過時間)を変更することができます．
    /// ただし，フレームレートをデフォルトの値から変更した際の動作の保証はしません．
    float seconds_per_frame = 0.001f;

    FCV1SimulatorFactory() = default;
    FCV1SimulatorFactory(FCV1SimulatorFactory const&); ///< コピーコンストラクタ
    FCV1SimulatorFactory & operator = (FCV1SimulatorFactory const&); ///< コピー代入演算子
    virtual ~FCV1SimulatorFactory() = default;

    virtual std::unique_ptr<ISimulator> CreateSimulator() const override;
    virtual std::unique_ptr<ISimulatorFactory> Clone() const override;
    virtual std::string GetSimulatorId() const override
    {
        return std::string(kFCV1SimulatorId);
    }
};


/// \cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, FCV1SimulatorFactory const&);
void from_json(nlohmann::json const&, FCV1SimulatorFactory &);
/// \endcond

} // namespace digital_curling::simulators

#endif // DIGITAL_CURLING_SIMULATORS_FCV1_SIMULATOR_FACTORY_HPP
