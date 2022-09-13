#include "config.hpp"
#include <cmath>
#include "util.hpp"

namespace digital_curling_server {

NLOHMANN_JSON_SERIALIZE_ENUM(Config::Game::Rule, {
    {Config::Game::Rule::kNormal, "normal"},
})


// --- Config ---

void to_json(nlohmann::json& j, Config const& config)
{
    namespace dc = digital_curling;

    {
        auto& j_server = j["server"];
        {
            auto& j_server_port = j_server["port"];
            for (size_t i = 0; i < 2; ++i) {
                j_server_port[dc::ToString(static_cast<dc::Team>(i))] = config.server.port[i];
            }
        }
        j_server["timeout_dc_ok"] = config.server.timeout_dc_ok;
        j_server["send_trajectory"] = config.server.send_trajectory;
        j_server["steps_per_trajectory_frame"] = config.server.steps_per_trajectory_frame;
    }

    {
        auto& j_game = j["game"];
        j_game["rule"] = config.game.rule;
        j_game["setting"] = config.game.setting;
        j_game["simulator"] = *config.game.simulator;
        {
            auto& j_game_players = j_game["players"];
            for (size_t i = 0; i < 2; ++i) {
                auto& j_game_players_teamx = j_game_players[dc::ToString(static_cast<dc::Team>(i))];
                for (auto const& player : config.game.players[i]) {
                    j_game_players_teamx.push_back(*player);
                }
            }
        }
    }

    j["game_is_ready"] = config.game_is_ready;
}



void from_json(nlohmann::json const& j, Config & config)
{
    namespace dc = digital_curling;

    {
        auto const& j_server = j.at("server");
        {
            auto const& j_server_port = j_server.at("port");
            for (size_t i = 0; i < 2; ++i) {
                j_server_port.at(dc::ToString(static_cast<dc::Team>(i))).get_to(config.server.port[i]);
            }
        }
        j_server.at("timeout_dc_ok").get_to(config.server.timeout_dc_ok);
        j_server.at("send_trajectory").get_to(config.server.send_trajectory);
        j_server.at("steps_per_trajectory_frame").get_to(config.server.steps_per_trajectory_frame);
    }

    {
        auto const& j_game = j.at("game");
        j_game.at("rule").get_to(config.game.rule);
        j_game.at("setting").get_to(config.game.setting);
        config.game.simulator = j_game.at("simulator").get<std::unique_ptr<digital_curling::ISimulatorFactory>>();
        {
            auto const& j_game_players = j_game.at("players");
            for (size_t i = 0; i < 2; ++i) {
                auto const& j_game_players_teamx = j_game_players.at(dc::ToString(static_cast<dc::Team>(i)));
                for (auto const& j_game_players_teamx_x : j_game_players_teamx) {
                    config.game.players[i].emplace_back(j_game_players_teamx_x.get<std::unique_ptr<digital_curling::IPlayerFactory>>());
                }
            }
            switch (config.game.rule) {
                case Config::Game::Rule::kNormal:
                    for (auto const& player : config.game.players) {
                        if (player.size() != 4) {
                            throw std::runtime_error("config.game.player.size() != 4");
                        }
                    }
                    break;
                default:
                    assert(false);
            }
        }
    }

    // game_is_ready の構築

    // game_is_readyとgame_is_ready_patchの両方を指定した場合エラーとする
    if (j.contains("game_is_ready") && j.contains("game_is_ready_patch")) {
        throw std::runtime_error("specify only one of \"game_is_ready\" or \"game_is_ready_patch\"");
    }

    if (auto it = j.find("game_is_ready"); it != j.end()) {
        config.game_is_ready = *it;
    } else if (auto it = j.find("game_is_ready_patch"); it != j.end()) {
        config.game_is_ready = j.at("game").patch(*it);
    } else {
        config.game_is_ready = j.at("game");
    }
}


} // namespace digital_curling_server
