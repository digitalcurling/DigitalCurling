#ifndef DIGITAL_CURLING_SRC_PLAYERS_IDENTICAL_PLAYER_STORAGE_HPP
#define DIGITAL_CURLING_SRC_PLAYERS_IDENTICAL_PLAYER_STORAGE_HPP

#include "digital_curling/detail/i_player_storage.hpp"

namespace digital_curling::players {

class IdenticalPlayerStorage : public IPlayerStorage {
public:
    IdenticalPlayerStorage() = default;
    IdenticalPlayerStorage(IdenticalPlayerStorage const&) {}
    IdenticalPlayerStorage & operator = (IdenticalPlayerStorage const&) { return *this; }
    virtual ~IdenticalPlayerStorage() = default;

    /// \brief このストレージが持つ状態と同じ状態を持つプレイヤーを生成する
    ///
    /// \returns 生成されたプレイヤー
    virtual std::unique_ptr<IPlayer> CreatePlayer() const override;

    /// \brief 対応するプレイヤーのプレイヤーIDを得る．
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なる．
    ///
    /// \returns プレイヤーID
    virtual std::string GetPlayerId() const override;
};


// json
void to_json(nlohmann::json &, IdenticalPlayerStorage const&);
void from_json(nlohmann::json const&, IdenticalPlayerStorage &);

} // namespace digital_curling::players


#endif // DIGITAL_CURLING_SRC_PLAYERS_IDENTICAL_PLAYER_STORAGE_HPP
