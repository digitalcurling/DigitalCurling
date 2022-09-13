#ifndef DIGITAL_CURLING_SERVER_LOG_HPP
#define DIGITAL_CURLING_SERVER_LOG_HPP

#include <cstdint>
#include <string_view>
#include <fstream>
#include <mutex>
#include <variant>
#include <filesystem>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "nlohmann/json.hpp"

namespace digital_curling_server {

/// \brief サーバーのログはこのクラスの関数を介して出力される．
///
/// シングルトン
class Log {
public:

    Log(std::filesystem::path const& directory_path, bool verbose, bool debug);
    Log(Log const&) = delete;
    Log & operator = (Log const&) = delete;
    ~Log();

    struct Server {};
    static constexpr Server kServer{};
    struct Client {
        size_t id;
        Client(size_t id) : id(id) {}
        Client(Client const&) = default;
        Client & operator = (Client const&) = default;
    };
    using Target = std::variant<Server, Client>;

    /// \brief 通信内容の表示に用いる
    /// 
    /// \param from 送信元
    /// \param to 受信先
    /// \param message 通信内容
    static void Trace(Target const& from, Target const& to, std::string_view message);

    /// \brief デバッグ用のログ
    ///
    /// \param message ログ
    static void Debug(std::string_view message);

    /// \brief 試合ログを出す
    ///
    /// GUIで試合ログを表示するためのデータ
    ///
    /// \param json ログデータ
    static void Game(nlohmann::json const& json);

    /// \brief ショットのログを出す
    ///
    /// GUIで試合ログを表示するためのデータ
    ///
    /// \param json ログデータ
    /// \param end エンド番号
    /// \param shot ショット番号
    static void Shot(nlohmann::json const& json, std::uint8_t end, std::uint8_t shot);

    /// \brief CUIに表示するログを出す
    ///
    /// \param message 表示するログ
    static void Info(std::string_view message);

    /// \brief 警告ログを出す
    ///
    /// サーバーを終了する程では無い，異常な動作を検出した場合に使う．
    ///
    /// \param message 警告メッセージ
    static void Warning(std::string_view message);

    /// \brief エラーメッセージを出す
    ///
    /// 通信などにエラーが発生した場合用いる．
    /// このメッセージを出したらサーバーは速やかに終了する．
    ///
    /// \param message エラーメッセージ
    static void Error(std::string_view message);

    /// \brief ログを出せる状態にあるか？
    /// 
    /// \return ログが出せるなら \c true
    static bool IsValid();

private:
    static inline Log * instance_ = nullptr;
    std::filesystem::path const directory_path_;
    bool const verbose_;
    bool const debug_;

    std::mutex mutex_;
    uint64_t next_id_;  // ログのID値生成用
    bool directory_created_;
    std::ofstream file_all_;
    std::ofstream file_game_;
    nlohmann::ordered_json CreateDetailedLog(std::string_view tag, nlohmann::json const& json,
        boost::posix_time::ptime time);
    void CheckGameLogFileOpen();
    void CheckDirectoryCreated();
};

} // namespace digital_curling_server

#endif
