#include <filesystem>
#include <optional>
#include <string>
#include "digitalcurling/plugins/plugin_type.hpp"
#include "common.hpp"

#ifndef DIGITALCURLING_TEST_DIR
    #error "DIGITALCURLING_TEST_DIR is not defined"
#endif

namespace digitalcurling::test {

class DynamicTestBase : public ::testing::Test {
protected:
    std::string simulator_plugin_path_;
    std::string player_plugin_path_;

    void SetUp() override;
    void TearDown() override { }

    virtual std::optional<std::string> TryLoad(std::filesystem::path const& path, std::string const& expected_name) = 0;

public:
    inline bool IsSimPluginLoaded() const { return !simulator_plugin_path_.empty(); }
    inline bool IsPlayerPluginLoaded() const { return !player_plugin_path_.empty(); }
};

} // namespace digitalcurling::test
