#ifndef DIGITAL_CURLING_SERVER_CONFIG_HPP
#define DIGITAL_CURLING_SERVER_CONFIG_HPP

#include <array>
#include <vector>
#include <memory>
#include <chrono>
#include "nlohmann/json.hpp"
#include "digital_curling/digital_curling.hpp"

namespace digital_curling_server {

struct Config {
    struct Server {
        std::array<unsigned short, 2> port;
        std::chrono::milliseconds timeout_dc_ok;
        bool send_trajectory;
        size_t steps_per_trajectory_frame;
    } server;

    struct Game {
        enum class Rule {
            kNormal
        } rule;
        digital_curling::GameSetting setting;
        std::unique_ptr<digital_curling::ISimulatorFactory> simulator;
        std::array<
            std::vector<std::unique_ptr<digital_curling::IPlayerFactory>>,
            2> players;  // normalルールではプレイヤーは各チーム4人
    } game;

    // is_ready 時に"game"として送信するjson
    nlohmann::json game_is_ready;
};


// JSONへの変換
// こちらの機能はデバッグ用
void to_json(nlohmann::json &, Config const&);

// JSONからの変換
void from_json(nlohmann::json const&, Config &);


} // namespace digital_curling_server

#endif
