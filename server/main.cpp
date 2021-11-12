#include <ctime>
#include <memory>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "server.hpp"
#include "log.hpp"

using boost::asio::ip::tcp;
using namespace digital_curling;
using digital_curling::server::Log;
using nlohmann::json;

int main(int argc, char* const argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config file path>\n";
        return 1;
    }

    server::Setting server_setting;
    std::filesystem::path file_name;
    std::unique_ptr<Log> log;

    try {
        // ログディレクトリを作成
        auto const log_dir = std::filesystem::path("log");
        std::filesystem::create_directory(log_dir);

        // 日時を得る
        auto now = std::time(0);
        auto t = std::localtime(&now);
        if (t == nullptr) {
            throw std::runtime_error("std::localtime() returned nullptr.");
        }
        char date_time[32];
        std::strftime(date_time, sizeof(date_time), "%Y%m%d_%H%M%S", t);

        // ログファイルを作成
        std::ofstream file;
        for (int count = 0; !file.is_open(); ++count) {
            // ゲームIDを作成
            std::ostringstream game_id_buf;
            game_id_buf << date_time << '_' << std::setfill('0') << std::right << std::setw(3) << count;
            server_setting.game_id = game_id_buf.str();

            // ファイル名を作成
            file_name = log_dir / server_setting.game_id;
            file_name += ".log";

            // ファイルが存在しない場合は新規作成．
            // ファイルが存在する場合はオープン時に中身を消さない．
            file.open(file_name, std::ios_base::out | std::ios_base::app);
            if (!file) {
                continue;
            }

            // ファイルサイズを得る．
            file.seekp(0, std::ios_base::end);
            auto const file_size = file.tellp();

            // 既に書き込みがあった場合はcloseして次のファイル名にする．
            if (file_size != 0) {
                file.close();
            }
        }

        // ログファイル生成
        log = std::make_unique<Log>(std::move(file));

    } catch (std::exception & e) {
        std::cerr << "Exception while creating log file: " << e.what() << std::endl;
        return 1;
    }

    try {
        Log::Debug() << "log file path: " << std::filesystem::absolute(file_name);
        Log::Debug() << "game id: " << server_setting.game_id;

        // TODO 起動設定ファイルを読み込む．
        std::ifstream config_file(argv[1]);
        json j_config;
        config_file >> j_config;

        auto port0 = j_config.at("port").at(0).get<unsigned short>();
        auto port1 = j_config.at("port").at(1).get<unsigned short>();

        if (j_config.contains("time_limit")) {
            server_setting.time_limit = std::chrono::seconds(j_config.at("time_limit").get<std::chrono::seconds::rep>());
        }
        
        if (j_config.contains("extra_time_limit")) {
            server_setting.extra_time_limit = std::chrono::seconds(j_config.at("extra_time_limit").get<std::chrono::seconds::rep>());
        }

        game::Setting game_setting;
        if (j_config.contains("game_setting")) {
            game_setting = j_config.at("game_setting").get<game::Setting>();
        }

        std::unique_ptr<simulation::ISimulatorSetting> simulator_setting;

        if (j_config.contains("simulator_setting")) {
            simulator_setting = j_config.at("simulator_setting").get<std::unique_ptr<simulation::ISimulatorSetting>>();
        } else {
            simulator_setting = std::make_unique<simulation::SimulatorFCV1Setting>();
        }

        boost::asio::io_context io_context;

        tcp::endpoint listen_endpoint0(tcp::v4(), port0);
        Log::Debug() << "port 0: " << listen_endpoint0.port();

        tcp::endpoint listen_endpoint1(tcp::v4(), port1);
        Log::Debug() << "port 1: " << listen_endpoint1.port();

        server::Server s(io_context, listen_endpoint0, listen_endpoint1, server_setting, game_setting, *simulator_setting);

        Log::Debug() << "Server started.";
        io_context.run();
        Log::Debug() << "Server terminated successfully.";

    } catch (std::exception & e) {
        Log::Error() << e.what();
        return 1;
    }

    return 0;

}
