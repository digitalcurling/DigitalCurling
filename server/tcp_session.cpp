#include "tcp_session.hpp"
#include <boost/asio/buffer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include "server.hpp"
#include "log.hpp"

namespace digital_curling_server {

using boost::asio::steady_timer;
using boost::asio::ip::tcp;

TCPSession::TCPSession(tcp::socket && socket, Server & server, size_t client_id)
    : server_(server)
    , socket_(std::move(socket))
    , client_id_(client_id)
    , input_deadline_(socket_.get_executor())
    , non_empty_output_queue_(socket_.get_executor())
    , last_output_time_(steady_timer::time_point::max())
{
    input_deadline_.expires_at(steady_timer::time_point::max());
}

void TCPSession::Open()
{
    ReadLine();
    CheckInputDeadline();
    AwaitOutput();

    server_.OnSessionStart(client_id_);
}

void TCPSession::Deliver(std::string && message, std::optional<std::chrono::milliseconds> const& input_timeout)
{
    message.push_back('\n');
    output_queue_.emplace_back(std::move(message), input_timeout); // TODO これやめたい．．
    non_empty_output_queue_.expires_at(steady_timer::time_point::min());
}

void TCPSession::Close()
{
    if (IsClosed()) return;

    boost::system::error_code ignored_error;
    socket_.close(ignored_error);
    input_deadline_.cancel();
    non_empty_output_queue_.cancel();

    {
        std::ostringstream buf;
        buf << "Client " << client_id_ << "'s session was stopped.";
        Log::Debug(buf.str());
    }

    // ここは例外発生時にも呼び出される．
    // 例外発生後はOnSessionStop()とかの関数を呼び出している場合じゃない．
}

bool TCPSession::IsClosed() const
{
    return !socket_.is_open();
}

void TCPSession::ReadLine()
{
    boost::asio::async_read_until(socket_,
        boost::asio::dynamic_buffer(input_buffer_), '\n',
        [this, self = shared_from_this()](boost::system::error_code const& error, std::size_t n)
        {
            if (IsClosed()) {
                return;
            }

            if (error) {
                // この部分はクライアントが正常に接続を解除した際にも呼ばれる
                std::ostringstream buf;
                buf << "Client " << client_id_ << "'s session will be stopped (ReadLine). - " << error.message();
                Log::Debug(buf.str());
                server_.OnSessionStop(client_id_);
                Close();
                return;
            }

            auto read_time = steady_timer::clock_type::now();
            std::chrono::milliseconds elapsed_from_output;
            if (last_output_time_ == steady_timer::time_point::max()) {
                elapsed_from_output = std::chrono::milliseconds(0);
            } else {
                elapsed_from_output = std::chrono::duration_cast<std::chrono::milliseconds>(read_time - last_output_time_);
            }

            // 入力タイムアウトが起こらないようにする．
            input_deadline_.expires_at(steady_timer::time_point::max());

            std::string_view msg(input_buffer_.c_str(), n - 1);  // メッセージを取得

            // 通信ログ．(文字列が長すぎる場合は文字数だけにする．)
            {
                Log::Trace(Log::Client(client_id_), Log::kServer, msg);
                std::ostringstream buf;
                buf << "client " << client_id_ << ": elapsed_from_output=" << elapsed_from_output.count() << "ms, msg_length=" << msg.size();
                Log::Debug(buf.str());
            }

            server_.OnSessionRead(client_id_, msg, elapsed_from_output);

            // 読み取り完了したのでバッファから削除
            // msgをstring_viewにしている都合，input_buffer_からの削除は読み取り完了後にする必要がある
            input_buffer_.erase(0, n);

            ReadLine();
        });
}

void TCPSession::AwaitOutput()
{
    non_empty_output_queue_.async_wait(
        [this, self = shared_from_this()](boost::system::error_code const& /* error */)
        {
            if (IsClosed()) {
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
            if (IsClosed()) {
                return;
            }

            if (error) { // 書き込み失敗はエラーなのでサーバーを停止
                std::ostringstream buf;
                buf << "client " << client_id_ << " write line - " << error.message();
                Log::Error(buf.str());
                server_.Stop();
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
                Log::Trace(Log::kServer, Log::Client(client_id_), str);
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
            if (IsClosed()) {
                return;
            }

            if (input_deadline_.expiry() <= steady_timer::clock_type::now()) {
                server_.OnSessionTimeout(client_id_);

                // タイムアウト発生後はタイムアウトが起こらないようにする．
                input_deadline_.expires_at(steady_timer::time_point::max());
            }

            CheckInputDeadline();
        }
    );
}

} // namespace digital_curling_server
