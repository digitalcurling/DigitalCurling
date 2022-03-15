#ifndef DIGITAL_CURLING_SERVER_TCP_SESSION_HPP
#define DIGITAL_CURLING_SERVER_TCP_SESSION_HPP

#include <deque>
#include <chrono>
#include <optional>
#include <memory>
#include <string>
#include <exception>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>

namespace digital_curling_server {

class Server;

class TCPSession : public std::enable_shared_from_this<TCPSession> {
public:
    TCPSession(boost::asio::ip::tcp::socket && socket, Server & server, size_t client_id);
    void Open();

    /// <summary>
    /// メッセージを送信する．
    /// </summary>
    /// <param name="message">送信するメッセージ．</param>
    /// <param name="input_timeout">次回の入力までのタイムアウト．nulloptの場合タイムアウトは発生しない．</param>
    void Deliver(std::string && message, std::optional<std::chrono::milliseconds> const& input_timeout);
    void Close();
    bool IsClosed() const;

private:
    struct Message {
        std::string message;
        std::optional<std::chrono::milliseconds> input_timeout;

        Message(std::string const& m, std::optional<std::chrono::milliseconds> const& t)
            : message(m)
            , input_timeout(t) {}
    };

    void ReadLine();
    void AwaitOutput();
    void WriteLine();
    void CheckInputDeadline();

    Server & server_;
    boost::asio::ip::tcp::socket socket_;
    size_t const client_id_;
    std::string input_buffer_;
    boost::asio::steady_timer input_deadline_;
    std::deque<Message> output_queue_;
    boost::asio::steady_timer non_empty_output_queue_;
    boost::asio::steady_timer::time_point last_output_time_;
};

} // namespace digital_curling_server



#endif // DIGITAL_CURLING_SERVER_TCP_SESSION_HPP
