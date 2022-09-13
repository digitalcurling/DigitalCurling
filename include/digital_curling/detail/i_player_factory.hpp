/// \file
/// \brief IPlayerFactory を定義します

#ifndef DIGITAL_CURLING_I_PLAYER_FACTORY_HPP
#define DIGITAL_CURLING_I_PLAYER_FACTORY_HPP

#include <memory>
#include <string>
#include "json/common.hpp"

namespace digital_curling {

class IPlayer;



/// \brief IPlayer を構築するためのクラスです
///
/// IPlayer のインスタンスはこのクラスの派生クラスの CreatePlayer() で生成されます．
class IPlayerFactory {
protected:
    IPlayerFactory() = default;
    IPlayerFactory(IPlayerFactory const&) = default;
    IPlayerFactory & operator = (IPlayerFactory const&) = default;

public:
    virtual ~IPlayerFactory() = default;

    /// \brief プレイヤーを生成する
    ///
    /// \return 生成されたプレイヤー
    virtual std::unique_ptr<IPlayer> CreatePlayer() const = 0;

    /// \brief このインスタンスを複製する
    ///
    /// \returns 複製されたインスタンス
    virtual std::unique_ptr<IPlayerFactory> Clone() const = 0;

    /// \brief 対応するプレイヤーのプレイヤーIDを得る．
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なる．
    ///
    /// \returns プレイヤーID
    virtual std::string GetPlayerId() const = 0;
};

} // namespace digital_curling



/// \cond Doxygen_Suppress
namespace digital_curling::polymorphic_json::detail {

template <>
ToJsonRegistry<IPlayerFactory> & GetToJsonRegistry<IPlayerFactory>();

template <>
FromJsonRegistry<IPlayerFactory> & GetFromJsonRegistry<IPlayerFactory>();

} // namespace digital_curling::polymorphic_json::detail
/// \endcond



/// \cond Doxygen_Suppress
namespace nlohmann {

template <>
struct adl_serializer<digital_curling::IPlayerFactory> :
    digital_curling::polymorphic_json::detail::Serializer<digital_curling::IPlayerFactory> {};

template <>
struct adl_serializer<std::unique_ptr<digital_curling::IPlayerFactory>> :
    digital_curling::polymorphic_json::detail::Serializer<std::unique_ptr<digital_curling::IPlayerFactory>> {};

/// \endcond

} // namespace nlohmann

#endif // DIGITAL_CURLING_I_PLAYER_FACTORY_HPP
