/// \file
/// \brief IdenticalPlayerFactory を定義します

#ifndef DIGITAL_CURLING_PLAYERS_IDENTICAL_PLAYER_FACTORY_HPP
#define DIGITAL_CURLING_PLAYERS_IDENTICAL_PLAYER_FACTORY_HPP

#include <memory>
#include "../json/common.hpp"
#include "../i_player_factory.hpp"

namespace digital_curling::players {

/// \brief IdenticalプレイヤーのID
constexpr std::string_view kIdenticalPlayerId = "identical";


/// \brief 乱数を加えないプレイヤー
class IdenticalPlayerFactory : public IPlayerFactory {
public:
    IdenticalPlayerFactory() = default;  ///< デフォルトコンストラクタ
    virtual ~IdenticalPlayerFactory() = default; 
    IdenticalPlayerFactory(IdenticalPlayerFactory const&) {}  ///< コピーコンストラクタ
    IdenticalPlayerFactory & operator = (IdenticalPlayerFactory const&) { return *this; }  ///< コピー代入演算子

    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;
    virtual std::unique_ptr<IPlayerFactory> Clone() const override;
    virtual std::string GetPlayerId() const override
    {
        return std::string(kIdenticalPlayerId);
    }
};


/// \cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, IdenticalPlayerFactory const&);
void from_json(nlohmann::json const&, IdenticalPlayerFactory &);
/// \endcond

} // namespace digital_curling::players

#endif // DIGITAL_CURLING_PLAYERS_IDENTICAL_PLAYER_FACTORY_HPP
