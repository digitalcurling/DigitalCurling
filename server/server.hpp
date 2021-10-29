#ifndef DIGITAL_CURLING_SERVER_SERVER_HPP
#define DIGITAL_CURLING_SERVER_SERVER_HPP

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <array>
#include <optional>
#include <chrono>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include "digital_curling/digital_curling.hpp"

namespace digital_curling::server {

class TCPSession;

struct Setting {
    std::string game_id;

    /// <summary>
    /// エクストラではないエンドのタイムリミット
    /// </summary>
    std::chrono::seconds time_limit{ 38 * 60 };

    /// <summary>
    /// エクストラエンド(1エンド毎)のタイムリミット
    /// </summary>
    std::chrono::seconds extra_time_limit{ 4 * 60 + 30 };
};

// -----

class Channel {
public:
    Channel(
        std::function<void()> && stop_accept,
        Setting const& server_setting,
        normal_game::Setting const& game_setting,
        simulation::ISimulatorSetting const& simulator_setting);

    void Join(size_t client_id, std::shared_ptr<TCPSession> const& tcp_session);
    void Leave(size_t client_id);

    void StartContact(size_t client_id);
    void OnRead(size_t client_id, std::string && message, std::chrono::seconds const& elapsed_from_output);
    void OnInputTimeout(size_t client_id);

private:
    struct Client {
        size_t const id;
        std::string name;
        std::shared_ptr<TCPSession> session;
        enum class State {
            kBeforeContact,
            kVersionCheck,
            kReadyCheck,
            kNewGame,
            kMyTurn,
            kOpponentTurn,
            kGameOver,
        } state;
        std::chrono::seconds remaining_time;

        Client(size_t id, std::chrono::seconds const& remaining_time);
    };
    friend std::ostream& operator << (std::ostream&, Client const&);

    void StopServer();
    void DeliverMessage(size_t client_id, std::string const& message, std::optional<std::chrono::seconds> const& input_timeout = std::nullopt);
    void Update(normal_game::Move & move);
    void DeliverUpdateMessage(std::optional<normal_game::Move> const& last_move);
    void DeliverGameOverMessage();

    std::function<void()> const stop_accept_;
    Setting server_setting_;
    std::array<Client, 2> clients_;
    std::unique_ptr<simulation::ISimulator> const simulator_;
    normal_game::Setting const game_setting_;
    normal_game::State game_state_;
    std::optional<std::array<std::optional<Vector2>, kStoneMax>> last_end_stone_positions_;
};

// -----

class TCPSession : public std::enable_shared_from_this<TCPSession> {
public:
    TCPSession(boost::asio::ip::tcp::socket && socket, Channel & channel, size_t client_id);
    void Start();

    /// <summary>
    /// メッセージを送信する．
    /// </summary>
    /// <param name="message">送信するメッセージ．</param>
    /// <param name="input_timeout">次回の入力までのタイムアウト．nulloptの場合タイムアウトは発生しない．</param>
    void Deliver(std::string const& message, std::optional<std::chrono::seconds> const& input_timeout);
    void Stop();
    bool IsStopped() const;

private:
    struct Message {
        std::string message;
        std::optional<std::chrono::seconds> input_timeout;

        Message(std::string const& m, std::optional<std::chrono::seconds> const& t)
            : message(m)
            , input_timeout(t) {}
    };

    void ReadLine();
    void AwaitOutput();
    void WriteLine();
    void CheckInputDeadline();

    Channel & channel_;
    boost::asio::ip::tcp::socket socket_;
    size_t const client_id_;
    std::string input_buffer_;
    boost::asio::steady_timer input_deadline_;
    std::deque<Message> output_queue_;
    boost::asio::steady_timer non_empty_output_queue_;
    boost::asio::steady_timer::time_point last_output_time_;
};

// -----

class Server {
public:

    Server(boost::asio::io_context & io_context,
        boost::asio::ip::tcp::endpoint const& listen_endpoint0,
        boost::asio::ip::tcp::endpoint const& listen_endpoint1,
        Setting const& server_setting,
        normal_game::Setting const& game_setting,
        simulation::ISimulatorSetting const& simulator_setting);

private:
    void AcceptOnce(size_t client_id);
    void StopAccept();
    std::array<boost::asio::ip::tcp::acceptor, 2> acceptors_;
    Channel channel_;
};

}

#endif
