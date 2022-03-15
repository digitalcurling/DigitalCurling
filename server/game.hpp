#ifndef DIGITAL_CURLING_SERVER_GAME_HPP
#define DIGITAL_CURLING_SERVER_GAME_HPP

#include <memory>
#include <array>
#include "digital_curling/digital_curling.hpp"
#include "config.hpp"
#include "trajectory_compressor.hpp"

namespace digital_curling_server {

class Server;

class Game {
public:
    Game(Server & server, Config && config, std::string const& date_time, std::string const& game_id);

    void OnSessionStart(size_t client_id);
    void OnSessionRead(size_t client_id, std::string_view input_message, std::chrono::milliseconds const& elapsed_from_output);
    void OnSessionTimeout(size_t client_id);
    void OnSessionStop(size_t client_id);

    Config const& GetConfig() const { return config_; }

private:
    struct Client {
        enum class State {
            kBeforeSessionStart, ///< ~ dc
            kDC,       ///< dc ~ dc_ok
            kReady,    ///< is_ready ~ ready_ok
            kNewGame,  ///< ready_ok ~ new_game
            kMyTurn,
            kOpponentTurn,
            kGameOver,
        };
        State state = State::kBeforeSessionStart;
        std::string name;
        std::vector<std::unique_ptr<digital_curling::IPlayer>> players;
        std::vector<size_t> player_order;
    };

    Server & server_;
    Config config_;
    std::string const date_time_;
    std::string const game_id_;

    nlohmann::json const json_dc_;
    nlohmann::json json_is_ready_;

    std::array<Client, 2> clients_;

    std::unique_ptr<digital_curling::ISimulator> const simulator_;
    digital_curling::GameState game_state_;

    TrajectoryCompressor compressor_;
    bool last_move_has_value_;
    bool last_move_free_guard_zone_foul_;
    nlohmann::json json_last_move_actual_move_;
    nlohmann::json json_last_move_trajectory_;

    void DoApplyMove(size_t moving_client_id, digital_curling::Move && move, std::chrono::milliseconds const& elapsed);
    void DeliverUpdateMessage();
};



} // namespace digital_curling_server

#endif // DIGITAL_CURLING_SERVER_GAME_HPP
