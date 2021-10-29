#include "server.hpp"
#include <atomic>
#include "nlohmann/json.hpp"
#include "log.hpp"

namespace digital_curling::server {

using boost::asio::steady_timer;
using boost::asio::ip::tcp;
using nlohmann::json;
using Buf = std::ostringstream;

namespace {

constexpr int kProtocolVersion = 1;
constexpr auto kVersionCheckTimeout = std::chrono::seconds(10);

inline size_t ToClientId(normal_game::TeamId team_id)
{
    switch (team_id) {
        case normal_game::TeamId::k0: return 0;
        case normal_game::TeamId::k1: return 1;
        default: assert(false); return -1;
    }
}

inline size_t OpponentClient(size_t client_id)
{
    if (client_id == 0) {
        return 1;
    } else if (client_id == 1) {
        return 0;
    } else {
        assert(false);
        return -1;
    }
}

} // unnamed namespace

Channel::Channel(std::function<void()> && stop_accept, Setting const& server_setting, normal_game::Setting const& game_setting, simulation::ISimulatorSetting const& simulator_setting)
    : stop_accept_(std::move(stop_accept))
    , server_setting_(server_setting)
    , clients_{ {
        Client(0, server_setting_.time_limit),
        Client(1, server_setting_.time_limit) } }
    , simulator_(simulator_setting.CreateSimulator())
    , game_setting_(game_setting)
    , game_state_()
{}

void Channel::Join(size_t client_id, std::shared_ptr<TCPSession> const& tcp_session)
{
    assert(client_id < clients_.size());
    clients_[client_id].session = tcp_session;
}

void Channel::Leave(size_t client_id)
{
    assert(client_id < clients_.size());
    clients_[client_id].session.reset();

    Log::Debug() << "Client " << client_id << " was left from channel.";

    if (clients_[client_id].state != Client::State::kGameOver) {
        // 正常なタイミングでの終了ではない
        Log::Error() << "Client " << client_id << " was left from channel at inappropriate time.";
        StopServer();
    }
    // 正常なタイミングの終了の場合，これ以上することは無い．
}

void Channel::StartContact(size_t client_id)
{
    assert(clients_.at(client_id).state == Client::State::kBeforeContact);

    Log::Debug() << "Start contact with client " << client_id;

    clients_.at(client_id).state = Client::State::kVersionCheck;

    json const j = {
        { "cmd", "dc" },
        { "version", kProtocolVersion },
    };

    auto const message = j.dump();

    if (client_id == 0) {
        Log::Info() << message;
    }

    DeliverMessage(client_id, message, kVersionCheckTimeout);
}

void Channel::OnRead(size_t client_id, std::string && input_message, std::chrono::seconds const& elapsed_time)
{
    try {
        switch (clients_.at(client_id).state) {
            case Client::State::kBeforeContact: {
                Buf buf;
                buf << clients_[client_id] << ": Received message from client before contact start.";
                throw std::runtime_error(buf.str());
                break;
            }

            case Client::State::kVersionCheck: {
                // receive dc_ok message
                auto const jin = json::parse(std::move(input_message));
                if (jin.at("cmd").get<std::string>() != "dc_ok") {
                    Buf buf;
                    buf << clients_[client_id] << ": Unexpected cmd. (expected: \"dc_ok\")";
                    throw std::runtime_error(buf.str());
                }
                if (jin.at("version").get<decltype(kProtocolVersion)>() != kProtocolVersion) {
                    Buf buf;
                    buf << clients_[client_id] << ": Version error.";
                    throw std::runtime_error(buf.str());
                }

                clients_[client_id].state = Client::State::kReadyCheck;

                // deliver is_ready message
                json const jout = {
                    { "cmd", "is_ready" },
                    { "game_id", server_setting_.game_id },
                    { "rule", "normal" },
                    { "game_setting", game_setting_ },
                    { "simulator_setting", simulator_->GetSetting() },
                    { "team_id", client_id },
                    { "time_limit", server_setting_.time_limit.count() },
                    { "extra_time_limit", server_setting_.extra_time_limit.count() },
                };
                auto const message = jout.dump();

                if (client_id == 0) {
                    Log::Info() << message;
                }

                DeliverMessage(client_id, message);
                break;
            }

            case Client::State::kReadyCheck: {
                // receive ready_ok message
                json const jin = json::parse(std::move(input_message));
                if (jin.at("cmd").get<std::string>() != "ready_ok") {
                    Buf buf;
                    buf << clients_[client_id] << ": Unexpected cmd. (expected: \"ready_ok\")";
                    throw std::runtime_error(buf.str());
                }

                jin.at("name").get_to(clients_[client_id].name);

                clients_[client_id].state = Client::State::kNewGame;

                if (clients_[0].state == Client::State::kNewGame
                    && clients_[1].state == Client::State::kNewGame) {
                    // start new game
                    // deliver new_game message
                    json const jout_new_game = {
                        { "cmd", "new_game" },
                        { "teams", json::array({
                            json::object({ { "name", clients_[0].name } }),
                            json::object({ { "name", clients_[1].name } }) }) },
                    };
                    auto const message = jout_new_game.dump();

                    Log::Info() << message;

                    for (auto const& client : clients_) {
                        DeliverMessage(client.id, message);
                    }

                    DeliverUpdateMessage(std::nullopt);
                }

                break;
            }

            case Client::State::kMyTurn: {
                // receive move message
                json const jin = json::parse(std::move(input_message));
                if (jin.at("cmd").get<std::string>() != "move") {
                    Buf buf;
                    buf << clients_[client_id] << ": Unexpected cmd. (expected: \"move\")";
                    throw std::runtime_error(buf.str());
                }

                // add elapsed time (thinking time)
                auto const moved_client = ToClientId(game_state_.GetCurrentTeam());
                clients_[moved_client].remaining_time -= elapsed_time;

                normal_game::Move move = jin.get<normal_game::Move>();

                // update client's states and deliver message
                Update(move);
                DeliverUpdateMessage(move);

                // if game is over, ...
                if (game_state_.result) {
                    DeliverGameOverMessage();
                    // 以降，クライアントの接続解除を待つ．
                }

                break;
            }

            case Client::State::kOpponentTurn: {
                Buf buf;
                buf << clients_[client_id] << ": Received message in opponent turn.";
                throw std::runtime_error(buf.str());
                break;
            }

            case Client::State::kGameOver: {
                // nothing to do
                Log::Warn() << "client " << client_id << "'s message was ignored.";
                break;
            }

            default:
                assert(false);
        }
    } catch (std::exception & e) {
        Log::Error() << e.what();
        StopServer();
    }
}

Channel::Client::Client(size_t id, std::chrono::seconds const& remaining_time)
    : id(id)
    , name()
    , session()
    , state(State::kBeforeContact)
    , remaining_time(remaining_time)
{}

std::ostream& operator << (std::ostream & o, Channel::Client const& client)
{
    o << "client(id=" << client.id;
    o << ", name=\"" << client.name << "\"";

    o << ", session=";
    if (client.session) {
        if (client.session->IsStopped()) {
            o << "stopped";
        } else {
            o << "connected";
        }
    } else {
        o << "null";
    }

    o << ", state=";
    switch (client.state) {
        case Channel::Client::State::kBeforeContact:
            o << "BeforeContact";
            break;
        case Channel::Client::State::kVersionCheck:
            o << "VersionCheck";
            break;
        case Channel::Client::State::kReadyCheck:
            o << "ReadyCheck";
            break;
        case Channel::Client::State::kNewGame:
            o << "NewGame";
            break;
        case Channel::Client::State::kMyTurn:
            o << "MyTurn";
            break;
        case Channel::Client::State::kOpponentTurn:
            o << "OpponentTurn";
            break;
        case Channel::Client::State::kGameOver:
            o << "GameOver";
            break;
        default:
            assert(false);
            break;
    }

    o << ", remaining_time=" << client.remaining_time.count();
    o << ")";

    return o;
}

void Channel::StopServer()
{
    stop_accept_();

    for (auto & client : clients_) {
        if (client.session) {
            client.session->Stop();
            client.session.reset();
        }
    }

    Log::Debug() << "Server was stopped.";
}

void Channel::OnInputTimeout(size_t client_id)
{
    switch (clients_.at(client_id).state) {
        case Client::State::kMyTurn: {
            Log::Debug() << "Client " << client_id << " timed out.";
            // 制限時間切れにより負け．
            clients_[client_id].remaining_time = std::chrono::seconds(0);
            normal_game::Move move = normal_game::move::TimeLimit();
            Update(move);
            DeliverUpdateMessage(move);
            DeliverGameOverMessage();
            break;
        }

        default:
            Log::Error() << "Client " << client_id << " timed out at an inappropriate time.";
            StopServer();
            break;
    }
}

void Channel::DeliverMessage(size_t client_id, std::string const& message, std::optional<std::chrono::seconds> const& input_timeout)
{
    assert(client_id < clients_.size());
    if (clients_[client_id].session && !clients_[client_id].session->IsStopped()) {
        clients_[client_id].session->Deliver(message, input_timeout);
    } else {
        Buf buf;
        buf << clients_[client_id] << ": Deliver message failed.";
        throw std::runtime_error(buf.str());
    }
}

void Channel::Update(normal_game::Move & move)
{
    auto current_client = ToClientId(game_state_.GetCurrentTeam());
    if (clients_[current_client].remaining_time.count() <= 0) {
        Log::Debug() << "Client " << current_client << " lost the game because of the time limit.";
        move = normal_game::move::TimeLimit();
    }

    {
        json j = move;
        j["cmd"] = "move";
        j["team"] = current_client;
        Log::Info() << j.dump();
    }

    normal_game::ApplyMove(game_setting_, game_state_, *simulator_, move);

    // エンド終了時にストーン位置を報告する．
    if (game_state_.current_shot == 0 && game_state_.current_end > 0) {  // エンド終了直後か？
        last_end_stone_positions_.emplace();
        auto const& stones = simulator_->GetStones();
        auto const prev_end_side = coordinate::GetShotSide(game_state_.current_end - 1);
        for (size_t i = 0; i < kStoneMax; ++i) {
            if (stones[i]) {
                (*last_end_stone_positions_)[i] = coordinate::TransformPosition(stones[i]->position, coordinate::Id::kSimulation, prev_end_side);
            } else {
                (*last_end_stone_positions_)[i] = std::nullopt;
            }
        }
    } else {
        last_end_stone_positions_ = std::nullopt;
    }

    // 延長エンドの残り時間を設定．
    if (game_state_.current_end >= game_setting_.end && game_state_.current_shot == 0) {
        for (auto & client : clients_) {
            client.remaining_time = server_setting_.extra_time_limit;
        }
    }
}

void Channel::DeliverUpdateMessage(std::optional<normal_game::Move> const& last_move)
{
    // update states
    auto const next_turn_client = ToClientId(game_state_.GetCurrentTeam());
    auto const opponent_next_turn = OpponentClient(next_turn_client);
    clients_[next_turn_client].state = Client::State::kMyTurn;
    clients_[opponent_next_turn].state = Client::State::kOpponentTurn;

    // 残り時間
    json remaining_times;
    for (auto const& client : clients_) {
        remaining_times[client.id] = client.remaining_time.count();
    }

    json const jout_update = {
        { "cmd", "update" },
        { "state", game_state_ },
        { "remaining_times", std::move(remaining_times) },
        { "last_move", last_move },
        { "last_end_stone_positions", last_end_stone_positions_ }
    };

    auto const message = jout_update.dump();

    Log::Info() << message;

    DeliverMessage(next_turn_client, message, clients_[next_turn_client].remaining_time);
    DeliverMessage(opponent_next_turn, message);
}

void Channel::DeliverGameOverMessage()
{
    // update clients' state
    for (auto & client : clients_) {
        client.state = Client::State::kGameOver;
    }

    // deliver game_over message
    json const jout_game_over = {
        {"cmd", "game_over"}
    };
    auto const message = jout_game_over.dump();

    Log::Info() << message;

    for (auto & client : clients_) {
        DeliverMessage(client.id, message);
    }
}

TCPSession::TCPSession(boost::asio::ip::tcp::socket && socket, Channel & channel, size_t client_id)
    : channel_(channel)
    , socket_(std::move(socket))
    , client_id_(client_id)
    , input_deadline_(socket_.get_executor())
    , non_empty_output_queue_(socket_.get_executor())
    , last_output_time_(steady_timer::time_point::max())
{
    input_deadline_.expires_at(steady_timer::time_point::max());
}

void TCPSession::Start()
{
    channel_.Join(client_id_, shared_from_this());

    ReadLine();
    CheckInputDeadline();

    AwaitOutput();

    channel_.StartContact(client_id_);
}

void TCPSession::Deliver(std::string const& message, std::optional<std::chrono::seconds> const& input_timeout)
{
    output_queue_.emplace_back(message + '\n', input_timeout);
    non_empty_output_queue_.expires_at(steady_timer::time_point::min());
}

void TCPSession::Stop()
{
    if (IsStopped()) return;

    boost::system::error_code ignored_error;
    socket_.close(ignored_error);
    input_deadline_.cancel();
    non_empty_output_queue_.cancel();

    Log::Debug() << "Client " << client_id_ << "'s session was stopped.";
}

bool TCPSession::IsStopped() const
{
    return !socket_.is_open();
}

void TCPSession::ReadLine()
{
    boost::asio::async_read_until(socket_,
        boost::asio::dynamic_buffer(input_buffer_), '\n',
        [this, self = shared_from_this()](boost::system::error_code const& error, std::size_t n)
        {
            if (IsStopped()) {
                return;
            }

            if (error) {
                // このエラーはクライアントが正常に接続を解除した際にも呼ばれる
                Log::Debug() << "error in client " << client_id_ << " read line. " << error.message();
                channel_.Leave(client_id_);
                Stop();
                return;
            }

            auto read_time = steady_timer::clock_type::now();
            std::chrono::seconds elapsed_from_output;
            if (last_output_time_ == steady_timer::time_point::max()) {
                elapsed_from_output = std::chrono::seconds(0);
            } else {
                elapsed_from_output = std::chrono::duration_cast<std::chrono::seconds>(read_time - last_output_time_);
            }

            // 入力タイムアウトが起こらないようにする．
            input_deadline_.expires_at(steady_timer::time_point::max());

            std::string msg(input_buffer_.substr(0, n - 1));
            input_buffer_.erase(0, n);

            // 通信ログ．(文字列が長すぎる場合は文字数だけにする．)
            {
                Buf buf;
                buf << "[in] client=" << client_id_ << ", elapsed_from_output=" << elapsed_from_output.count() << "s, message=";
                if (msg.size() <= 500) {
                    buf << msg;
                } else {
                    buf << "(too long (size=" << msg.size() << "))";
                }
                Log::Trace() << buf.str();
            }

            channel_.OnRead(client_id_, std::move(msg), elapsed_from_output);

            ReadLine();
        });
}

void TCPSession::AwaitOutput()
{
    non_empty_output_queue_.async_wait(
        [this, self = shared_from_this()](boost::system::error_code const& /* error */)
        {
            if (IsStopped()) {
                return;
            }

            if (output_queue_.empty()) {
                non_empty_output_queue_.expires_at(steady_timer::time_point::max());
                AwaitOutput();
            } else {
                WriteLine();
            }
        });
}

void TCPSession::WriteLine()
{
    boost::asio::async_write(socket_,
        boost::asio::buffer(output_queue_.front().message),
        [this, self = shared_from_this()](boost::system::error_code const& error, std::size_t /* n */)
        {
            if (IsStopped()) {
                return;
            }

            if (error) {
                // このエラーはクライアントが正常に接続を解除した際にも呼ばれる
                Log::Error() << "error in client " << client_id_ << " write line. " << error.message();
                channel_.Leave(client_id_);
                Stop();
                return;
            }

            last_output_time_ = steady_timer::clock_type::now();

            // input_deadline_ の設定
            Message const& message = output_queue_.front();
            if (message.input_timeout) {
                input_deadline_.expires_after(*message.input_timeout);
            } else {
                input_deadline_.expires_at(steady_timer::time_point::max());
            }

            {
                std::string_view str = std::string_view(message.message).substr(0, message.message.size() - 1);
                Log::Trace() << "[out] client=" << client_id_ << ", message=" << str;
            }

            output_queue_.pop_front();
            AwaitOutput();
        });
}

void TCPSession::CheckInputDeadline()
{
    input_deadline_.async_wait(
        [this, self = shared_from_this()](const boost::system::error_code& /*error*/)
        {
            if (IsStopped()) {
                return;
            }

            if (input_deadline_.expiry() <= steady_timer::clock_type::now()) {
                channel_.OnInputTimeout(client_id_);
            }

            CheckInputDeadline();
        }
    );
}

Server::Server(boost::asio::io_context & io_context,
    boost::asio::ip::tcp::endpoint const& listen_endpoint0,
    boost::asio::ip::tcp::endpoint const& listen_endpoint1,
    Setting const& server_setting,
    normal_game::Setting const& game_setting,
    simulation::ISimulatorSetting const& simulator_setting)
    : acceptors_{{ { io_context, listen_endpoint0 }, { io_context, listen_endpoint1 } }}
    , channel_([this] { StopAccept(); }, server_setting, game_setting, simulator_setting)
{
    AcceptOnce(0);
    AcceptOnce(1);
}

void Server::AcceptOnce(size_t client_id)
{
    acceptors_.at(client_id).async_accept(
        [this, client_id](boost::system::error_code const& error, tcp::socket && socket)
        {
            if (!error) {
                std::make_shared<TCPSession>(std::move(socket), channel_, client_id)->Start();
            }
        });
}

void Server::StopAccept()
{
    for (auto & acceptor : acceptors_) {
        acceptor.cancel();
    }
}

}

