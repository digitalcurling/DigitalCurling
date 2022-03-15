#include <string>
#include <sstream>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <optional>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <boost/program_options.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "digital_curling/digital_curling.hpp"

#include "log.hpp"
#include "util.hpp"
#include "config.hpp"


namespace digital_curling_server {

void Start(Config && config, std::string const& launch_time, std::string const& game_id);

} // namespace digital_curling_server

using namespace std::string_view_literals;
namespace dcs = digital_curling_server;
using dcs::Log;
namespace po = boost::program_options;

int main(int argc, char* const argv[])
{
    constexpr auto kDefaultConfigPath = "./config.json"sv;
    constexpr auto kServerVersion = "0.1"sv;
    constexpr auto kManualURL = "http://github.com/digitalcurling/DigitalCurling"sv;

    std::optional<Log> log_instance;

    try {

        // --- ゲームID，起動時刻の確定 ---

        auto const launch_time = boost::posix_time::second_clock::local_time();
        auto const game_uuid_str = boost::uuids::to_string(boost::uuids::random_generator()());
        std::filesystem::path const log_directory = [&] {
            std::ostringstream buf;
            buf << dcs::GetISO8601String(launch_time) << '_' << game_uuid_str << '/';
            return std::filesystem::absolute(buf.str());
        }();


        // --- コマンドライン引数の解析 ---

        po::options_description opt_desc("Allowed options");

        {
            std::ostringstream buf_config_desc;
            buf_config_desc << "set config json file path (default: \"" << kDefaultConfigPath << "\")";
            opt_desc.add_options()
                ("help,h", "produce help message")
                ("config,C", po::value<std::string>(), buf_config_desc.str().c_str())
                ("version", "show version")
                ("verbose,v", "verbose command line")
                ("debug", "debug mode")
                ;
        }

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opt_desc), vm);
        po::notify(vm);

        bool const arg_verbose = vm.count("verbose");
        bool const arg_debug = vm.count("debug");

        log_instance.emplace(log_directory, arg_verbose, arg_debug); // ログシステムの起動

        {
            std::ostringstream buf;
            buf << "Digital Curling server ver." << kServerVersion;
            Log::Info(buf.str());
        }

        if (arg_debug) {
            Log::Debug("debug: on");
        } else {
            Log::Debug("debug: off");
        }

        if (arg_verbose) {
            Log::Debug("verbose: on");
        } else {
            Log::Debug("verbose: off");
        }

        if (vm.count("help")) {
            std::ostringstream buf;
            buf << "online manual: " << kManualURL << '\n' << opt_desc;
            Log::Info(buf.str());
            return 0;
        }

        if (vm.count("version")) {
            std::ostringstream buf;
            buf << "Digital Curling: " << digital_curling::GetVersion();
            Log::Info(buf.str());
            return 0;
        }

        std::filesystem::path const config_path = [&] {
            std::filesystem::path config_path_tmp;
            if (vm.count("config")) {
                auto config_path_str = vm["config"].as<std::string>();
                config_path_tmp = config_path_str;
                std::ostringstream buf;
                buf << "config: \"" << config_path_str << '\"';
                Log::Debug(buf.str());
            } else {
                config_path_tmp = kDefaultConfigPath;
                Log::Debug("config: (none)");
            }
            return std::filesystem::absolute(config_path_tmp); // 絶対パスに変換
        }();

        {
            std::ostringstream buf;
            buf << "config file   : \"" << config_path.string() << "\"";
            Log::Info(buf.str());
        }

        {
            std::ostringstream buf;
            buf << "log directory : \"" << log_directory.string() << "\"";
            Log::Info(buf.str());
        }



        // --- コンフィグのパース ---

        std::ifstream config_file(config_path);
        if (!config_file) {
            throw std::runtime_error("could not open config file");
        }
        auto const config_json = nlohmann::json::parse(config_file, nullptr, true, true); // ignore_comment: true
        dcs::Config config = config_json.get<dcs::Config>();

        // --- サーバーの起動 ---

        dcs::Start(std::move(config), dcs::GetISO8601ExtendedString(launch_time), game_uuid_str);

        Log::Info("server terminated successfully");


    } catch (std::exception & e) {
        std::ostringstream buf;
        buf << "exception: " << e.what();

        if (Log::IsValid()) {
            Log::Error(buf.str());
        } else {
            std::cerr << buf.str() << std::endl;
        }
    } catch (...) {
        constexpr auto msg = "unknown exception";
        if (Log::IsValid()) {
            Log::Error(msg);
        } else {
            std::cerr << msg << std::endl;
        }
    }


    return 0;


    // if (argc != 2) {
    //     std::cerr << "Usage: " << argv[0] << " <config file path>\n";
    //     return 1;
    // }

    // server::Setting server_setting;
    // std::filesystem::path file_name;
    // std::unique_ptr<Log> log;

    // try {
    //     // ログディレクトリを作成
    //     auto const log_dir = std::filesystem::path("log");
    //     std::filesystem::create_directory(log_dir);

    //     // 日時を得る
    //     auto now = std::time(0);
    //     auto t = std::localtime(&now);
    //     if (t == nullptr) {
    //         throw std::runtime_error("std::localtime() returned nullptr.");
    //     }
    //     char date_time[32];
    //     std::strftime(date_time, sizeof(date_time), "%Y%m%d_%H%M%S", t);

    //     // ログファイルを作成
    //     std::ofstream file;
    //     for (int count = 0; !file.is_open(); ++count) {
    //         // ゲームIDを作成
    //         std::ostringstream game_id_buf;
    //         game_id_buf << date_time << '_' << std::setfill('0') << std::right << std::setw(3) << count;
    //         server_setting.game_id = game_id_buf.str();

    //         // ファイル名を作成
    //         file_name = log_dir / server_setting.game_id;
    //         file_name += ".log";

    //         // ファイルが存在しない場合は新規作成．
    //         // ファイルが存在する場合はオープン時に中身を消さない．
    //         file.open(file_name, std::ios_base::out | std::ios_base::app);
    //         if (!file) {
    //             continue;
    //         }

    //         // ファイルサイズを得る．
    //         file.seekp(0, std::ios_base::end);
    //         auto const file_size = file.tellp();

    //         // 既に書き込みがあった場合はcloseして次のファイル名にする．
    //         if (file_size != 0) {
    //             file.close();
    //         }
    //     }

    //     // ログファイル生成
    //     log = std::make_unique<Log>(std::move(file));

    // } catch (std::exception & e) {
    //     std::cerr << "Exception while creating log file: " << e.what() << std::endl;
    //     return 1;
    // }

    // try {
    //     Log::Debug() << "log file path: " << std::filesystem::absolute(file_name);
    //     Log::Debug() << "game id: " << server_setting.game_id;

    //     // TODO 起動設定ファイルを読み込む．
    //     std::ifstream config_file(argv[1]);
    //     json j_config;
    //     config_file >> j_config;

    //     auto port0 = j_config.at("port").at(0).get<unsigned short>();
    //     auto port1 = j_config.at("port").at(1).get<unsigned short>();

    //     if (j_config.contains("time_limit")) {
    //         server_setting.time_limit = std::chrono::seconds(j_config.at("time_limit").get<std::chrono::seconds::rep>());
    //     }
    //     
    //     if (j_config.contains("extra_time_limit")) {
    //         server_setting.extra_time_limit = std::chrono::seconds(j_config.at("extra_time_limit").get<std::chrono::seconds::rep>());
    //     }

    //     game::Setting game_setting;
    //     if (j_config.contains("game_setting")) {
    //         game_setting = j_config.at("game_setting").get<game::Setting>();
    //     }

    //     std::unique_ptr<simulation::ISimulatorSetting> simulator_setting;

    //     if (j_config.contains("simulator_setting")) {
    //         simulator_setting = j_config.at("simulator_setting").get<std::unique_ptr<simulation::ISimulatorSetting>>();
    //     } else {
    //         simulator_setting = std::make_unique<simulation::SimulatorFCV1Setting>();
    //     }

    //     boost::asio::io_context io_context;

    //     tcp::endpoint listen_endpoint0(tcp::v4(), port0);
    //     Log::Debug() << "port 0: " << listen_endpoint0.port();

    //     tcp::endpoint listen_endpoint1(tcp::v4(), port1);
    //     Log::Debug() << "port 1: " << listen_endpoint1.port();

    //     server::Server s(io_context, listen_endpoint0, listen_endpoint1, server_setting, game_setting, *simulator_setting);

    //     Log::Debug() << "Server started.";
    //     io_context.run();
    //     Log::Debug() << "Server terminated successfully.";

    // } catch (std::exception & e) {
    //     Log::Error() << e.what();
    //     return 1;
    // }

    return 0;

}
