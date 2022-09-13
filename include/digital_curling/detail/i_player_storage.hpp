/// \file
/// \brief IPlayerStorage を定義します

#ifndef DIGITAL_CURLING_I_PLAYER_STORAGE_HPP
#define DIGITAL_CURLING_I_PLAYER_STORAGE_HPP

#include <memory>
#include <string>
#include "json/common.hpp"

namespace digital_curling {

class IPlayer;



/// \brief IPlayer の状態を保存するストレージ
///
/// IPlayer::Load() や IPlayerStorage::CreatePlayer() を用いてストレージから状態を復元した場合，
/// IPlayer は IPlayer::Save() や IPlayer::CreateStorage() を用いてセーブした時点の内部状態を完全に復元します．
/// 言い換えると，あるストレージに保存された状態を復元した後の IPlayer::Play() の呼出し結果は，そのストレージに保存した時点からの IPlayer::Play() の呼出し結果と同じものになります．
///
/// ストレージは IPlayer のそれぞれの実装と対応関係にあるため， GetPlayerId() で得られるIDが一致していない場合は保存/復元はできません．
class IPlayerStorage {
protected:
    IPlayerStorage() = default;
    IPlayerStorage(IPlayerStorage const&) = default;
    IPlayerStorage & operator = (IPlayerStorage const&) = default;

public:
    virtual ~IPlayerStorage() = default;

    /// \brief このストレージが持つ状態と同じ状態を持つプレイヤーを生成する
    ///
    /// \returns 生成されたプレイヤー
    virtual std::unique_ptr<IPlayer> CreatePlayer() const = 0;

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
ToJsonRegistry<IPlayerStorage> & GetToJsonRegistry<IPlayerStorage>();

template <>
FromJsonRegistry<IPlayerStorage> & GetFromJsonRegistry<IPlayerStorage>();

} // namespace digital_curling::polymorphic_json::detail
/// \endcond



/// \cond Doxygen_Suppress
// json
namespace nlohmann {

template <>
struct adl_serializer<digital_curling::IPlayerStorage> :
    digital_curling::polymorphic_json::detail::Serializer<digital_curling::IPlayerStorage> {};

template <>
struct adl_serializer<std::unique_ptr<digital_curling::IPlayerStorage>> :
    digital_curling::polymorphic_json::detail::Serializer<std::unique_ptr<digital_curling::IPlayerStorage>> {};


} // namespace nlohmann
/// \endcond

#endif // DIGITAL_CURLING_I_SIMULATOR_STORAGE_HPP
