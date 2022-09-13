/// \file
/// \brief NormalDistPlayerFactory を定義します

#ifndef DIGITAL_CURLING_PLAYERS_NORMAL_DIST_PLAYER_FACTORY_HPP
#define DIGITAL_CURLING_PLAYERS_NORMAL_DIST_PLAYER_FACTORY_HPP

#include <random>
#include <memory>
#include <optional>
#include "../json/common.hpp"
#include "../i_player_factory.hpp"

namespace digital_curling::players {

/// \brief Normal DistプレイヤーのID
constexpr std::string_view kNormalDistPlayerId = "normal_dist";


/// \brief ショットの初速と角度に正規分布の乱数を加えるプレイヤー
class NormalDistPlayerFactory : public IPlayerFactory {
public:
    /// \brief ショットの最大速度
    float max_speed = 4.f;

    /// \brief ショットの初速に加わる正規分布乱数の標準偏差．
    float stddev_speed = 0.0076f;

    /// \brief ショットの初期角度に加わる正規分布乱数の標準偏差．
    float stddev_angle = 0.0018f;

    /// \brief 乱数のシード値．
    ///
    /// \c std::nullopt の場合シード値を自動でランダムに選択する．
    std::optional<std::random_device::result_type> seed = std::nullopt;

    NormalDistPlayerFactory() = default;  ///< デフォルトコンストラクタ
    virtual ~NormalDistPlayerFactory() = default;
    NormalDistPlayerFactory(NormalDistPlayerFactory const&) = default;  ///< コピーコンストラクタ
    NormalDistPlayerFactory & operator = (NormalDistPlayerFactory const&) = default;  ///< コピー代入演算子

    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;
    virtual std::unique_ptr<IPlayerFactory> Clone() const override;
    virtual std::string GetPlayerId() const override
    {
        return std::string(kNormalDistPlayerId);
    }
};


/// \cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, NormalDistPlayerFactory const&);
void from_json(nlohmann::json const&, NormalDistPlayerFactory &);
/// \endcond

} // namespace digital_curling::players

#endif // DIGITAL_CURLING_PLAYERS_NORMAL_DIST_PLAYER_FACTORY_HPP
