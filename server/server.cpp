#include "server.hpp"
#include <boost/asio/io_context.hpp>
#include "log.hpp"
#include "util.hpp"

namespace digital_curling_server {

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context & io_context, Config && config, std::string const& date_time, std::string const& game_id)
    : listen_endpoints_()
    , acceptors_()
    , sessions_()
    , game_(*this, std::move(config), date_time, game_id)
{
    for (size_t i = 0; i < 2; ++i) {
        listen_endpoints_[i].emplace(tcp::v4(), game_.GetConfig().server.port[i]);
        acceptors_[i].emplace(io_context, *listen_endpoints_[i]);

        // accept
        acceptors_.at(i)->async_accept(
            [this, i](boost::system::error_code const& error, tcp::socket && socket)
            {
                if (!error) {
                    sessions_[i] = std::make_shared<TCPSession>(std::move(socket), *this, i);
                    sessions_[i]->Open();
                }
            });
    }
}

void Server::Stop()
{
    // stop accept
    for (auto & acceptor : acceptors_) {
        acceptor->cancel();
    }

    for (auto & session : sessions_) {
        if (session) {
            session->Close();
            session.reset();
        }
    }

    Log::Debug("server stopped");
}

void Server::OnSessionStop(size_t client_id)
{
    sessions_[client_id].reset();

    try {
        game_.OnSessionStop(client_id);
    } catch (std::exception & e) {
        HandleError(e);
    }
}

void Server::OnSessionStart(size_t client_id)
{
    try {
        game_.OnSessionStart(client_id);
    } catch (std::exception & e) {
        HandleError(e);
    }
}

void Server::OnSessionRead(size_t client_id, std::string_view input_message, std::chrono::milliseconds const& elapsed_from_output)
{
    try {
        game_.OnSessionRead(client_id, input_message, elapsed_from_output);
    } catch (std::exception & e) {
        HandleError(e);
    }
}

void Server::OnSessionTimeout(size_t client_id)
{
    try {
        game_.OnSessionTimeout(client_id);
    } catch (std::exception & e) {
        HandleError(e);
    }
}

void Server::DeliverMessage(size_t client_id, std::string && message, std::optional<std::chrono::milliseconds> const& input_timeout)
{
    if (sessions_[client_id] && !sessions_[client_id]->IsClosed()) {
        sessions_[client_id]->Deliver(std::move(message), input_timeout);
    } else {
        std::ostringstream buf;
        buf << "client " << client_id << " deliver message failed";
        throw std::runtime_error(buf.str());
    }
}


void Server::HandleError(std::exception & e)
{
    Log::Error(e.what());
    Stop();
}


void Start(Config && config, std::string const& launch_time, std::string const& game_id)
{
    {
        std::ostringstream buf;
        buf << "launch time: " << launch_time;
        Log::Info(buf.str());
    }

    {
        std::ostringstream buf;
        buf << "game id    : " << game_id;
        Log::Info(buf.str());
    }

    for (size_t i = 0; i < config.server.port.size(); ++i) {
        std::ostringstream buf;
        buf << "team " << i << " port: " << config.server.port[i];
        Log::Info(buf.str());
    }
    Log::Info("Note: Team 1 has the last stone in the first end.");

    boost::asio::io_context io_context;
    Server s(io_context, std::move(config), launch_time, game_id);

    Log::Info("server started");

    io_context.run();
}




// Channel::Channel(std::function<void()> && stop_accept, Config const& config)
//     : stop_accept_(std::move(stop_accept))
//     , config_(config)
//     , clients_{ {
//         Client(0, server_setting_.time_limit),
//         Client(1, server_setting_.time_limit) } }
//     , simulator_(simulator_setting.CreateSimulator())
//     , game_state_()
// {}
// 
// void Channel::Join(size_t client_id, std::shared_ptr<TCPSession> const& tcp_session)
// {
//     assert(client_id < clients_.size());
//     clients_[client_id].session = tcp_session;
// }
// 
// void Channel::Leave(size_t client_id)
// {
//     clients_[client_id].session.reset();
// 
//     Log::Debug() << "Client " << client_id << " was left from channel.";
// 
//     if (clients_[client_id].state != Client::State::kGameOver) {
//         // 正常なタイミングでの終了ではない
//         Log::Error() << "Client " << client_id << " was left from channel at inappropriate time.";
//         StopServer();
//     }
//     // 正常なタイミングの終了の場合，これ以上することは無い．
// }
// 
// void Channel::StartContact(size_t client_id)
// {
//     assert(clients_.at(client_id).state == Client::State::kBeforeContact);
// 
//     Log::Debug() << "Start contact with client " << client_id;
// 
//     clients_.at(client_id).state = Client::State::kVersionCheck;
// 
//     json const j = {
//         { "cmd", "dc" },
//         { "version", kProtocolVersion },
//     };
// 
//     auto const message = j.dump();
// 
//     if (client_id == 0) {
//         Log::Info() << message;
//     }
// 
//     DeliverMessage(client_id, message, kVersionCheckTimeout);
// }
// 
// void Channel::OnRead(size_t client_id, std::string && input_message, std::chrono::milliseconds const& elapsed_time)
// {
//     try {
//         switch (clients_.at(client_id).state) {
//             case Client::State::kBeforeContact: {
//                 Buf buf;
//                 buf << clients_[client_id] << ": Received message from client before contact start.";
//                 throw std::runtime_error(buf.str());
//                 break;
//             }
// 
//             case Client::State::kVersionCheck: {
//                 // receive dc_ok message
//                 auto const jin = json::parse(std::move(input_message));
//                 if (jin.at("cmd").get<std::string>() != "dc_ok") {
//                     Buf buf;
//                     buf << clients_[client_id] << ": Unexpected cmd. (expected: \"dc_ok\")";
//                     throw std::runtime_error(buf.str());
//                 }
//                 if (jin.at("version").get<decltype(kProtocolVersion)>() != kProtocolVersion) {
//                     Buf buf;
//                     buf << clients_[client_id] << ": Version error.";
//                     throw std::runtime_error(buf.str());
//                 }
// 
//                 clients_[client_id].state = Client::State::kReadyCheck;
// 
//                 // deliver is_ready message
//                 json const jout = {
//                     { "cmd", "is_ready" },
//                     { "game_id", server_setting_.game_id },
//                     { "rule", "normal" },
//                     { "game_setting", game_setting_ },
//                     { "simulator_setting", simulator_->GetSetting() },
//                     { "team", client_id },
//                     { "time_limit", server_setting_.time_limit.count() },
//                     { "extra_time_limit", server_setting_.extra_time_limit.count() },
//                 };
//                 auto const message = jout.dump();
// 
//                 if (client_id == 0) {
//                     Log::Info() << message;
//                 }
// 
//                 DeliverMessage(client_id, message);
//                 break;
//             }
// 
//             case Client::State::kReadyCheck: {
//                 // receive ready_ok message
//                 json const jin = json::parse(std::move(input_message));
//                 if (jin.at("cmd").get<std::string>() != "ready_ok") {
//                     Buf buf;
//                     buf << clients_[client_id] << ": Unexpected cmd. (expected: \"ready_ok\")";
//                     throw std::runtime_error(buf.str());
//                 }
// 
//                 jin.at("name").get_to(clients_[client_id].name);
// 
//                 clients_[client_id].state = Client::State::kNewGame;
// 
//                 if (clients_[0].state == Client::State::kNewGame
//                     && clients_[1].state == Client::State::kNewGame) {
//                     // start new game
//                     // deliver new_game message
//                     json const jout_new_game = {
//                         { "cmd", "new_game" },
//                         { "teams", json::array({
//                             json::object({ { "name", clients_[0].name } }),
//                             json::object({ { "name", clients_[1].name } }) }) },
//                     };
//                     auto const message = jout_new_game.dump();
// 
//                     Log::Info() << message;
// 
//                     for (auto const& client : clients_) {
//                         DeliverMessage(client.id, message);
//                     }
// 
//                     DeliverUpdateMessage(std::nullopt);
//                 }
// 
//                 break;
//             }
// 
//             case Client::State::kMyTurn: {
//                 // receive move message
//                 json const jin = json::parse(std::move(input_message));
//                 if (jin.at("cmd").get<std::string>() != "move") {
//                     Buf buf;
//                     buf << clients_[client_id] << ": Unexpected cmd. (expected: \"move\")";
//                     throw std::runtime_error(buf.str());
//                 }
// 
//                 // add elapsed time (thinking time)
//                 auto const moved_client = ToClientId(game_state_.GetCurrentTeam());
//                 clients_[moved_client].remaining_time -= elapsed_time;
// 
//                 game::Move move = jin.get<game::Move>();
// 
//                 // update client's states and deliver message
//                 Update(move);
//                 DeliverUpdateMessage(move);
// 
//                 // if game is over, ...
//                 if (game_state_.result) {
//                     DeliverGameOverMessage();
//                     // 以降，クライアントの接続解除を待つ．
//                 }
// 
//                 break;
//             }
// 
//             case Client::State::kOpponentTurn: {
//                 Buf buf;
//                 buf << clients_[client_id] << ": Received message in opponent turn.";
//                 throw std::runtime_error(buf.str());
//                 break;
//             }
// 
//             case Client::State::kGameOver: {
//                 // nothing to do
//                 Log::Warn() << "client " << client_id << "'s message was ignored.";
//                 break;
//             }
// 
//             default:
//                 assert(false);
//         }
//     } catch (std::exception & e) {
//         Log::Error() << e.what();
//         StopServer();
//     }
// }
// 
// Channel::Client::Client(size_t id, std::chrono::milliseconds const& remaining_time)
//     : id(id)
//     , name()
//     , session()
//     , state(State::kBeforeContact)
//     , remaining_time(remaining_time)
// {}
// 
// std::ostream& operator << (std::ostream & o, Channel::Client const& client)
// {
//     o << "client(id=" << client.id;
//     o << ", name=\"" << client.name << "\"";
// 
//     o << ", session=";
//     if (client.session) {
//         if (client.session->IsStopped()) {
//             o << "stopped";
//         } else {
//             o << "connected";
//         }
//     } else {
//         o << "null";
//     }
// 
//     o << ", state=";
//     switch (client.state) {
//         case Channel::Client::State::kBeforeContact:
//             o << "BeforeContact";
//             break;
//         case Channel::Client::State::kVersionCheck:
//             o << "VersionCheck";
//             break;
//         case Channel::Client::State::kReadyCheck:
//             o << "ReadyCheck";
//             break;
//         case Channel::Client::State::kNewGame:
//             o << "NewGame";
//             break;
//         case Channel::Client::State::kMyTurn:
//             o << "MyTurn";
//             break;
//         case Channel::Client::State::kOpponentTurn:
//             o << "OpponentTurn";
//             break;
//         case Channel::Client::State::kGameOver:
//             o << "GameOver";
//             break;
//         default:
//             assert(false);
//             break;
//     }
// 
//     o << ", remaining_time=" << client.remaining_time.count();
//     o << ")";
// 
//     return o;
// }
// 
// void Channel::StopServer()
// {
//     stop_accept_();
// 
//     for (auto & client : clients_) {
//         if (client.session) {
//             client.session->Stop();
//             client.session.reset();
//         }
//     }
// 
//     Log::Debug() << "Server was stopped.";
// }
// 
// void Channel::OnInputTimeout(size_t client_id)
// {
//     switch (clients_.at(client_id).state) {
//         case Client::State::kMyTurn: {
//             Log::Debug() << "Client " << client_id << " timed out.";
//             // 制限時間切れにより負け．
//             clients_[client_id].remaining_time = std::chrono::milliseconds(0);
//             game::Move move = game::moves::TimeLimit();
//             Update(move);
//             DeliverUpdateMessage(move);
//             DeliverGameOverMessage();
//             break;
//         }
// 
//         default:
//             Log::Error() << "Client " << client_id << " timed out at an inappropriate time.";
//             StopServer();
//             break;
//     }
// }
// 
// void Channel::DeliverMessage(size_t client_id, std::string const& message, std::optional<std::chrono::milliseconds> const& input_timeout)
// {
//     assert(client_id < clients_.size());
//     if (clients_[client_id].session && !clients_[client_id].session->IsStopped()) {
//         clients_[client_id].session->Deliver(message, input_timeout);
//     } else {
//         Buf buf;
//         buf << clients_[client_id] << ": Deliver message failed.";
//         throw std::runtime_error(buf.str());
//     }
// }
// 
// void Channel::Update(game::Move & move)
// {
//     auto current_client = ToClientId(game_state_.GetCurrentTeam());
//     if (clients_[current_client].remaining_time.count() <= 0) {
//         Log::Debug() << "Client " << current_client << " lost the game because of the time limit.";
//         move = game::moves::TimeLimit();
//     }
// 
//     {
//         json j = move;
//         j["cmd"] = "move";
//         j["team"] = current_client;
//         Log::Info() << j.dump();
//     }
// 
//     game::ApplyMove(game_setting_, game_state_, *simulator_, move);
// 
//     // エンド終了時にストーン位置を報告する．
//     if (game_state_.current_shot == 0 && game_state_.current_end > 0) {  // エンド終了直後か？
//         last_end_stone_positions_.emplace();
//         auto const& stones = simulator_->GetStones();
//         auto const prev_end_side = coordinate::GetShotSide(game_state_.current_end - 1);
//         for (size_t i = 0; i < kStoneMax; ++i) {
//             if (stones[i]) {
//                 (*last_end_stone_positions_)[i] = coordinate::TransformPosition(stones[i]->position, coordinate::Id::kSimulation, prev_end_side);
//             } else {
//                 (*last_end_stone_positions_)[i] = std::nullopt;
//             }
//         }
//     } else {
//         last_end_stone_positions_ = std::nullopt;
//     }
// 
//     // 延長エンドの残り時間を設定．
//     if (game_state_.current_end >= game_setting_.end && game_state_.current_shot == 0) {
//         for (auto & client : clients_) {
//             client.remaining_time = server_setting_.extra_time_limit;
//         }
//     }
// }
// 
// void Channel::DeliverUpdateMessage(std::optional<game::Move> const& last_move)
// {
//     // 残り時間
//     json remaining_times;
//     for (auto const& client : clients_) {
//         remaining_times[client.id] = client.remaining_time.count();
//     }
// 
//     json const jout_update = {
//         { "cmd", "update" },
//         { "state", game_state_ },
//         { "remaining_times", std::move(remaining_times) },
//         { "last_move", last_move },
//         { "last_end_stone_positions", last_end_stone_positions_ }
//     };
// 
//     auto const message = jout_update.dump();
// 
//     Log::Info() << message;
// 
//     if (game_state_.result) {
//         for (auto & client : clients_) {
//             client.state = Client::State::kGameOver;
//         }
//         DeliverMessage(0, message);
//         DeliverMessage(1, message);
//     } else {
//         auto const next_turn_client = ToClientId(game_state_.GetCurrentTeam());
//         auto const opponent_next_turn = OpponentClient(next_turn_client);
// 
//         clients_[next_turn_client].state = Client::State::kMyTurn;
//         clients_[opponent_next_turn].state = Client::State::kOpponentTurn;
// 
//         DeliverMessage(next_turn_client, message, clients_[next_turn_client].remaining_time);
//         DeliverMessage(opponent_next_turn, message);
//     }
// 
// }
// 
// void Channel::DeliverGameOverMessage()
// {
//     // deliver game_over message
//     json const jout_game_over = {
//         {"cmd", "game_over"}
//     };
//     auto const message = jout_game_over.dump();
// 
//     Log::Info() << message;
// 
//     for (auto & client : clients_) {
//         DeliverMessage(client.id, message);
//     }
// }

} // namespace digital_curling_server

