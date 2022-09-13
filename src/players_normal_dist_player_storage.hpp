#ifndef DIGITAL_CURLING_SRC_PLAYERS_NORMAL_DIST_PLAYER_STORAGE_HPP
#define DIGITAL_CURLING_SRC_PLAYERS_NORMAL_DIST_PLAYER_STORAGE_HPP

#include "digital_curling/detail/i_player_storage.hpp"
#include "digital_curling/detail/players/normal_dist_player_factory.hpp"

namespace digital_curling::players {

class NormalDistPlayerStorage : public IPlayerStorage {
public:
    NormalDistPlayerStorage() = default;
    NormalDistPlayerStorage(NormalDistPlayerStorage const&) = default;
    NormalDistPlayerStorage & operator = (NormalDistPlayerStorage const&) = default;
    virtual ~NormalDistPlayerStorage() = default;

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

    NormalDistPlayerFactory factory;
    std::string engine_data;
    std::string speed_dist_data;
    std::string angle_dist_data;
};


// json
void to_json(nlohmann::json &, NormalDistPlayerStorage const&);
void from_json(nlohmann::json const&, NormalDistPlayerStorage &);

} // namespace digital_curling::players


#endif // DIGITAL_CURLING_SRC_PLAYERS_NORMAL_DIST_PLAYER_STORAGE_HPP
