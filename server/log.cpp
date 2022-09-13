#include "log.hpp"
#include <cassert>
#include <iostream>
#include <exception>
#include <thread>
#include <string_view>
#include <charconv>
#include "util.hpp"

namespace digital_curling_server {

namespace {

using namespace std::string_view_literals;

constexpr auto kAllLogFile  = "server.log"sv;
constexpr auto kGameLogFile = "game.dcl2"sv;
inline std::string GetShotLogFile(uint8_t end, uint8_t shot)
{
    std::ostringstream buf;
    buf << "shot_e" << std::setw(3) << std::setfill('0') << static_cast<unsigned int>(end)
        << "s" << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(shot) << ".json";
    return buf.str();
}

constexpr auto kTagTrace   = "trc"sv;
constexpr auto kTagDebug   = "dbg"sv;
constexpr auto kTagInfo    = "inf"sv;
constexpr auto kTagGame    = "gam"sv;
constexpr auto kTagShot    = "sht"sv;
constexpr auto kTagWarning = "wrn"sv;
constexpr auto kTagError   = "err"sv;

constexpr auto kTargetServer = "server"sv;
constexpr auto kTargetClient = "client"sv;


// overloadedトリック用ヘルパークラス
// 参考: https://dev.to/tmr232/that-overloaded-trick-overloading-lambdas-in-c17
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...)->Overloaded<Ts...>;

std::string ToString(Log::Target const& target)
{
    std::string str;
    std::visit(
        Overloaded{
            [&](Log::Server const&) {
                str = kTargetServer;
            },
            [&](Log::Client const& c) {
                std::ostringstream buf;
                buf << kTargetClient << c.id;
                str = buf.str();
            }
        },
        target);
    return str;
}



void PutLineHeader(std::ostream & o, std::string_view header, std::string_view message)
{
    size_t line_end = message.find_first_of('\n');
    while (true) {
        o << header;
        if (line_end == std::string_view::npos) {
            o << message;
            break;
        }
        o << message.substr(0, line_end + 1);
        message.remove_prefix(line_end + 1);
        line_end = message.find_first_of('\n');
    }
}

void PutMessage(std::ostream & o, boost::posix_time::ptime time, std::string_view header, std::string_view message)
{
    std::ostringstream buf_header;
    buf_header << '[' << GetTimeOfDay(time) << "] " << header;
    PutLineHeader(std::cout, buf_header.str(), message);
    std::cout << std::endl;
}



} // unnamed namespace


Log::Log(std::filesystem::path const& directory_path, bool verbose, bool debug)
    : directory_path_(directory_path)
    , verbose_(verbose)
    , debug_(debug)
    , mutex_()
    , next_id_(0)
    , directory_created_(false)
    , file_all_(kAllLogFile.data())
    , file_game_()
{
    assert(instance_ == nullptr);
    instance_ = this;

    // check directory_path
    if (std::filesystem::exists(directory_path)) {
        throw std::runtime_error("log directory already exists");
    }
}

Log::~Log()
{
    instance_ = nullptr;
}

void Log::Trace(Target const& from, Target const& to, std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();

    nlohmann::ordered_json const json{
        { "from", ToString(from) },
        { "to", ToString(to) },
        { "msg", message }
    };

    auto const detailed = instance_->CreateDetailedLog(kTagTrace, json, t);

    instance_->file_all_ << detailed << std::endl;
}

void Log::Debug(std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagDebug, message, t);

    if (instance_->debug_) {
        if (instance_->verbose_) {
            std::cout << detailed << std::endl;
        } else {
            PutMessage(std::cout, t, "[debug] ", message);
        }
    }

    // all
    instance_->file_all_ << detailed << std::endl;
}

void Log::Info(std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagInfo, message, t);
    
    // stdout
    if (instance_->verbose_) {
        std::cout << detailed << std::endl;
    } else {
        PutMessage(std::cout, t, "", message);
    }

    // all
    instance_->file_all_ << detailed << std::endl;
}

void Log::Game(nlohmann::json const& json)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagGame, json, t);

    instance_->CheckGameLogFileOpen();

    if (instance_->verbose_) {
        std::cout << detailed << std::endl;
    }

    instance_->file_game_ << detailed << std::endl;
    instance_->file_all_ << detailed << std::endl;
}

void Log::Shot(nlohmann::json const& json, std::uint8_t end, std::uint8_t shot)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagShot, json, t);

    instance_->CheckDirectoryCreated();

    std::ofstream file(instance_->directory_path_ / GetShotLogFile(end, shot));
    file << detailed.dump(2) << std::endl;

    instance_->file_all_ << detailed << std::endl;
}

void Log::Warning(std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagWarning, message, t);

    PutMessage(std::cerr, t, "[warning] ", message);

    // all
    instance_->file_all_ << detailed << std::endl;
}

void Log::Error(std::string_view message)
{
    assert(instance_);
    std::lock_guard g(instance_->mutex_);

    boost::posix_time::ptime const t = boost::posix_time::second_clock::local_time();
    auto const detailed = instance_->CreateDetailedLog(kTagError, message, t);

    PutMessage(std::cerr, t, "[error] ", message);

    // all
    instance_->file_all_ << detailed << std::endl;

    // gameログファイルが開かれているなら，エラーメッセージを出す
    if (instance_->file_game_) {
        instance_->file_game_ << detailed << std::endl;
    }
}

bool Log::IsValid()
{
    return instance_ != nullptr;
}

nlohmann::ordered_json Log::CreateDetailedLog(std::string_view tag, nlohmann::json const& json, boost::posix_time::ptime time)
{
    std::ostringstream buf_thread_id;
    buf_thread_id << std::this_thread::get_id();

    nlohmann::ordered_json j{
        { "tag", tag },
        { "id", next_id_ },
        { "date_time", GetISO8601ExtendedString(time) },
        { "thread", buf_thread_id.str() },
        { "log", json }
    };

    ++next_id_;

    return j;
}

void Log::CheckGameLogFileOpen()
{
    CheckDirectoryCreated();

    if (!file_game_.is_open()) {  // operator bool で判定すると初回にスキップされるため is_open() で判定する
        file_game_.open(directory_path_ / kGameLogFile);
    }
}

void Log::CheckDirectoryCreated()
{
    if (directory_created_) return;

    std::filesystem::create_directories(directory_path_);

    directory_created_ = true;
}


} // namespace digital_curling_server
