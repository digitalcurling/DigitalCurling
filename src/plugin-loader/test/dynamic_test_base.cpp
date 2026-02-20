#include <filesystem>
#include <iostream>
#include <string>
#include "digitalcurling/plugins/plugin_manager.hpp"
#include "dynamic_test_base.hpp"

void digitalcurling::test::DynamicTestBase::SetUp() {
    std::filesystem::path base_plugin_dir = std::filesystem::path(DIGITALCURLING_TEST_DIR);

    const char* plugin_dir_env = std::getenv("DIGITALCURLING_PLUGIN_DIR");
    if (plugin_dir_env && std::filesystem::exists(base_plugin_dir / plugin_dir_env)) {
        base_plugin_dir = (base_plugin_dir / plugin_dir_env).lexically_normal();
        std::cout << "Using plugin directory from env (DIGITALCURLING_PLUGIN_DIR): ";
#ifdef DIGITALCURLING_TEST_PLUGINS_DIR
    } else if (std::filesystem::exists(std::filesystem::path(DIGITALCURLING_TEST_PLUGINS_DIR))) {
        base_plugin_dir = std::filesystem::path(DIGITALCURLING_TEST_PLUGINS_DIR).lexically_normal();
        std::cout << "Using plugin directory from macro (DIGITALCURLING_TEST_PLUGINS_DIR): ";
#endif
    } else {
        base_plugin_dir = (base_plugin_dir / "plugins").lexically_normal();
        std::cout << "Using plugin directory default path: ";
    }
    std::cout << base_plugin_dir.string() << std::endl;

    std::filesystem::path sim_path = base_plugin_dir /
        ("digitalcurling_simulator_" + std::string(kSimPluginName) + digitalcurling::plugins::LibraryExtension);
    std::filesystem::path player_path = base_plugin_dir /
        ("digitalcurling_player_" + std::string(kPlayerPluginName) + digitalcurling::plugins::LibraryExtension);

    auto sim_res = TryLoad(sim_path, std::string(kSimPluginName));
    if (sim_res.has_value()) {
        std::cerr << "Warning (Setup): Failed to load simulator plugin: " << sim_res.value() << std::endl;
    } else {
        simulator_plugin_path_ = sim_path.string();
    }

    auto player_res = TryLoad(player_path, std::string(kPlayerPluginName));
    if (player_res.has_value()) {
        std::cerr << "Warning (Setup): Failed to load player plugin: " << player_res.value() << std::endl;
    } else {
        player_plugin_path_ = player_path.string();
    }
}
