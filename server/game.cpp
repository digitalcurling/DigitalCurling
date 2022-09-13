#include "game.hpp"

#include <boost/asio/ip/host_name.hpp>
#include "server.hpp"
#include "log.hpp"

namespace digital_curling_server {

namespace dc = digital_curling;
using nlohmann::json;
using namespace std::string_view_literals;

namespace {

constexpr int kProtocolVersionMajor = 2;
constexpr int kProtocolVersionMinor = 0;

inline void ThrowRuntimeError(size_t client_id, std::string_view message)
{
    std::ostringstream buf;
    buf << "client " << client_id << ": " << message;
    throw std::runtime_error(buf.str());
}

inline void CheckCommand(size_t client_id, nlohmann::json const& jin, std::string_view expected_command)
{
    if (jin.at("cmd").get<std::string>() != expected_command) {
        std::ostringstream buf;
        buf << "unexpected command (expected: \"" << expected_command << "\")";
        ThrowRuntimeError(client_id, buf.str());
    }
}

inline void LogInfoClient(size_t client_id, std::string_view message)
{
    std::ostringstream buf;
    buf << "client " << client_id << ": " << message;
    Log::Info(buf.str());
}


} // unnamed namespace

Game::Game(Server & server, Config && config, std::string const& date_time, std::string const& game_id)
    : server_(server)
    , config_(std::move(config))
    , date_time_(date_time)
    , game_id_(game_id)
    , json_dc_{
        { "cmd", "dc" },
        { "version", {
            { "major", kProtocolVersionMajor },
            { "minor", kProtocolVersionMinor },
        }},
        { "game_id", game_id_ },
        { "date_time", date_time_ } }
    , json_is_ready_{
        { "cmd", "is_ready" },
        { "game", config_.game_is_ready } }
    , clients_{{}}
    , simulator_(config_.game.simulator->CreateSimulator())
    , game_state_(config_.game.setting)
    , compressor_()
    , last_move_has_value_(false)
    , last_move_free_guard_zone_foul_(false)
    , json_last_move_actual_move_()
    , json_last_move_trajectory_()
{
    // rule

    if (config_.game.rule != Config::Game::Rule::kNormal) {
        throw std::invalid_argument("unsupported rule");
    }

    // init players

    for (size_t i = 0; i < clients_.size(); ++i) {
        for (auto const& player_factory : config_.game.players[i]) {
            clients_[i].players.emplace_back(player_factory->CreatePlayer());
        }
    }
}

void Game::OnSessionStart(size_t client_id)
{
    assert(clients_.at(client_id).state == Client::State::kBeforeSessionStart);

    clients_.at(client_id).state = Client::State::kDC;

    LogInfoClient(client_id, "start connection");

    // send dc
    server_.DeliverMessage(client_id, json_dc_.dump(), config_.server.timeout_dc_ok);
}


void Game::OnSessionRead(size_t client_id, std::string_view input_message, std::chrono::milliseconds const& elapsed_from_output)
{
    assert(client_id < clients_.size());

    switch (clients_[client_id].state) {
        case Client::State::kBeforeSessionStart: {
            ThrowRuntimeError(client_id, "received message before contact start");
            break;
        }

        case Client::State::kDC: {
            // receive dc_ok message
            auto const jin = json::parse(std::move(input_message));
            CheckCommand(client_id, jin, "dc_ok"sv);

            // input name
            clients_[client_id].name = jin.at("name").get<std::string>();

            // update state
            clients_[client_id].state = Client::State::kReady;

            LogInfoClient(client_id, "dc_ok");

            // deliver is_ready
            json_is_ready_["team"] = static_cast<dc::Team>(client_id);
            server_.DeliverMessage(client_id, json_is_ready_.dump());
            break;
        }

        case Client::State::kReady: {
            // receive ready_ok
            json const jin = json::parse(std::move(input_message));
            CheckCommand(client_id, jin, "ready_ok"sv);

            // input player_order
            assert(clients_[client_id].player_order.empty());
            for (auto const& j_player_idx : jin.at("player_order")) {
                clients_[client_id].player_order.emplace_back(j_player_idx.get<size_t>());
            }
            if (clients_[client_id].player_order.size() != clients_[client_id].players.size()) {
                ThrowRuntimeError(client_id, "invalid player_order size");
            }

            // プレイヤー順序のかぶりをチェック
            if (clients_[client_id].player_order.size() >= 2) {
                for (size_t i = 0; i < clients_[client_id].player_order.size() - 1; ++i) {
                    for (size_t j = i + 1; j < clients_[client_id].player_order.size(); ++j) {
                        if (clients_[client_id].player_order[i] == clients_[client_id].player_order[j]) {
                            ThrowRuntimeError(client_id, "player_order is overlapping");
                            goto END_ORDER_CHECK;
                        }
                    }
                }
                END_ORDER_CHECK: ;
            }

            // update state
            clients_[client_id].state = Client::State::kNewGame;

            LogInfoClient(client_id, "ready_ok");

            // start new game if both teams are ready
            if (clients_[0].state == Client::State::kNewGame
                && clients_[1].state == Client::State::kNewGame) {
                // start new game

                // Gameログへの記録を開始する(この時点でファイルが作成される)

                // Gameログにdcコマンドを書き出す
                Log::Game(json_dc_);

                // meta spec (コンピュータの情報)
                {
                    json const json_meta_spec{
                        { "cmd", "meta" },
                        { "meta", "spec" },
                        { "host_name", boost::asio::ip::host_name() }
                    };

                    Log::Game(json_meta_spec);
                }

                // meta config (入力されたコンフィグの内容と再現用の完全な設定をGameログに書き出す)
                {
                    json json_meta_config{
                        { "cmd", "meta" },
                        { "meta", "config" },
                    };
                    json_meta_config["config"] = config_;

                    // config.game.simulatorを実際のシミュレータをもとに更新
                    config_.game.simulator = simulator_->GetFactory().Clone();

                    // config.game.playersを実際のプレイヤーをもとに更新
                    for (size_t i_team = 0; i_team < 2; ++i_team) {
                        auto const& players = clients_[i_team].players;
                        auto & config_players = config_.game.players[i_team];
                        config_players.clear();
                        for (size_t i_player = 0; i_player < players.size(); ++i_player) {
                            config_players.emplace_back(players[i_player]->GetFactory().Clone());
                        }
                    }

                    // config_allに書き出す
                    json_meta_config["config_all"] = config_;

                    Log::Game(json_meta_config);
                }

                // Gameログ: dc_ok
                {
                    json json_dc_ok{
                        { "cmd", "dc_ok" }
                    };
                    for (size_t i = 0; i < 2; ++i) {
                        json_dc_ok["name"] = clients_[i].name;
                        json_dc_ok["team"] = static_cast<dc::Team>(i);
                        Log::Game(json_dc_ok);
                    }
                }

                // Gameログにis_readyコマンドを書き出す
                // ただし，"team"の内容はnullとしておく
                json_is_ready_["team"] = nullptr;
                Log::Game(json_is_ready_);

                // ready_ok
                {
                    json json_ready_ok = {
                        { "cmd", "ready_ok" }
                    };
                    for (size_t i = 0; i < 2; ++i) {
                        json_ready_ok["team"] = static_cast<dc::Team>(i);
                        json_ready_ok["player_order"] = clients_[i].player_order;
                        Log::Game(json_ready_ok);
                    }
                }
                
                // deliver new_game message

                json jout_new_game = {
                    { "cmd", "new_game" },
                };
                for (size_t i = 0; i < 2; ++i) {
                    jout_new_game["name"][dc::ToString(static_cast<dc::Team>(i))] = clients_[i].name;
                }

                Log::Game(jout_new_game);
                
                {
                    std::ostringstream buf;
                    buf << "team 0: \"" << clients_[0].name << "\"\n"
                        << "team 1: \"" << clients_[1].name << "\"\n"
                        << "game start";
                    Log::Info(buf.str());
                }

                for (size_t i = 0; i < clients_.size(); ++i) {
                    server_.DeliverMessage(i, jout_new_game.dump());
                }

                DeliverUpdateMessage();
            }

            break;
        }

        case Client::State::kMyTurn: {
            // receive move
            json const jin = json::parse(std::move(input_message));
            CheckCommand(client_id, jin, "move"sv);

            DoApplyMove(client_id, jin.at("move").get<dc::Move>(), elapsed_from_output);
            DeliverUpdateMessage();

            break;
        }

        case Client::State::kOpponentTurn: {
            ThrowRuntimeError(client_id, "received message in opponent turn");
            break;
        }

        case Client::State::kGameOver: {
            // nothing to do
            std::ostringstream buf;
            buf << "game was over. client " << client_id << "'s message is ignored.";
            Log::Warning(buf.str());
            break;
        }

        default:
            assert(false);
    }
}


void Game::OnSessionTimeout(size_t client_id)
{
    switch (clients_.at(client_id).state) {
        case Client::State::kMyTurn: {
            LogInfoClient(client_id, "time limit expired");

            // 制限時間切れにより負け．
            DoApplyMove(client_id, dc::moves::Concede(), std::chrono::milliseconds::max());  // ここのコンシードはダミーです．
            DeliverUpdateMessage();
            break;
        }

        default:
            ThrowRuntimeError(client_id, "timed out at an inappropriate time");
            break;
    }
}

void Game::OnSessionStop(size_t client_id)
{
    if (clients_[client_id].state != Client::State::kGameOver) {
        // 正常なタイミングでの終了ではない
        ThrowRuntimeError(client_id, "disconnected at inappropriate time");
    }
    // 正常なタイミングの終了の場合，特にすることは無い．
}

void Game::DoApplyMove(size_t moved_client_id, digital_curling::Move && move, std::chrono::milliseconds const& elapsed)
{
    dc::Team const moved_team = game_state_.GetNextTeam();
    assert(static_cast<dc::Team>(moved_client_id) == moved_team);

    // Gameログ: move
    {
        json const j_move = {
            { "cmd", "move" },
            { "move", move },
            { "team", static_cast<dc::Team>(moved_client_id) }
        };
        Log::Game(j_move);
    }

    // 現在のショット番号から投げるプレイヤーのインデックス(0: リード, 1: セカンド, 2: サード, 3: フォース を意味する)を得る
    // これはショット番号(0スタート)を4で割れば得られる
    auto const player_order_idx = game_state_.shot / 4;
    auto const player_idx = clients_[moved_client_id].player_order[player_order_idx];
    auto const& player = clients_[moved_client_id].players[player_idx];

    auto const move_end = game_state_.end;
    auto const move_shot = game_state_.shot;
    auto const selected_move = move;

    // trajectoryを送信しない場合でもログには軌跡を残すため，TrajectoryCompressorは必ず必要になる．
    compressor_.Begin(config_.server.steps_per_trajectory_frame, game_state_.end);

    dc::ApplyMoveResult apply_move_result;
    dc::ApplyMove(
        config_.game.setting,
        *simulator_,
        *player,
        game_state_,
        move,
        elapsed,
        &apply_move_result,
        [this](dc::ISimulator const& simulator) { compressor_.OnStep(simulator); });

    compressor_.End(*simulator_);

    last_move_has_value_ = true;
    last_move_free_guard_zone_foul_ = apply_move_result.free_guard_zone_foul;

    // ショットログの構築
    {
        json json_shot = {
            { "game_id", game_id_ },
            { "game_date_time", date_time_ },
            { "end", move_end },
            { "shot", move_shot },
            { "selected_move", selected_move },
            { "actual_move", move },
            { "trajectory", compressor_.GetResult() }
        };
        Log::Shot(json_shot, move_end, move_shot);
        json_last_move_actual_move_.swap(json_shot.at("actual_move"));
        json_last_move_trajectory_.swap(json_shot.at("trajectory"));
    }

    // スコア表示
    if (game_state_.shot == 0) { // 1エンド目以降しか反応しない
        for (size_t i = 0; i < 2; ++i) {
            std::ostringstream buf;
            buf << "team " << i << " score [";
            for (auto const& score : game_state_.scores[i]) {
                buf << ' ';
                if (score) {
                    buf << static_cast<std::uint32_t>(*score);
                } else {
                    buf << '-';
                }
            }
            buf << " | ";
            if (game_state_.extra_end_score[i]) {
                buf << static_cast<std::uint32_t>(*game_state_.extra_end_score[i]);
            } else {
                buf << '-';
            }
            buf << " ] total: ";
            buf << game_state_.GetTotalScore(static_cast<dc::Team>(i));
            Log::Info(buf.str());
        }
    }
}

void Game::DeliverUpdateMessage()
{
    json json_update = {
        { "cmd", "update" },
        { "next_team", game_state_.GetNextTeam() },
        { "state", game_state_ }
    };
    json & json_update_last_move = json_update["last_move"];

    if (last_move_has_value_) {
        json_update_last_move["actual_move"].swap(json_last_move_actual_move_);
        json_update_last_move["free_guard_zone_foul"] = last_move_free_guard_zone_foul_;
    }

    Log::Game(json_update);

    if (last_move_has_value_ && config_.server.send_trajectory) {
        json_update_last_move["trajectory"].swap(json_last_move_trajectory_);
    }

    auto update_message = json_update.dump();

    if (game_state_.game_result) {
        for (auto & client : clients_) {
            client.state = Client::State::kGameOver;
        }
        server_.DeliverMessage(0, std::string(update_message));
        server_.DeliverMessage(1, std::move(update_message));

        // deliver game_over message
        json const jout_game_over = {
            {"cmd", "game_over"}
        };
        Log::Game(jout_game_over);

        auto game_over_message = jout_game_over.dump();

        server_.DeliverMessage(0, std::string(game_over_message));
        server_.DeliverMessage(1, std::move(game_over_message));

        std::ostringstream buf;
        buf << "game over\nwin: " << dc::ToString(game_state_.game_result->winner);
        Log::Info(buf.str());
    } else {
        auto const next_turn_client = game_state_.GetNextTeam();
        auto const opponent_next_turn = dc::GetOpponentTeam(next_turn_client);

        clients_[static_cast<size_t>(next_turn_client)].state = Client::State::kMyTurn;
        clients_[static_cast<size_t>(opponent_next_turn)].state = Client::State::kOpponentTurn;

        server_.DeliverMessage(static_cast<size_t>(next_turn_client), std::string(update_message), game_state_.thinking_time_remaining[static_cast<size_t>(next_turn_client)]);
        server_.DeliverMessage(static_cast<size_t>(opponent_next_turn), std::move(update_message));


        // コマンドラインに出力
        {
            std::ostringstream buf;
            buf << "end: " << int(game_state_.end) << " (" << int(game_state_.end) + 1 << "/" << int(config_.game.setting.max_end) << ")"
                << ", shot: " << int(game_state_.shot)
                << ", turn: " << dc::ToString(game_state_.GetNextTeam());
            Log::Info(buf.str());
        }

    }
}


} // namespace digital_curling_server
