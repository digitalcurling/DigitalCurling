#include <memory>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "digitalcurling/digitalcurling.hpp"

namespace dc = digitalcurling;
namespace dcp = digitalcurling::players;
using nlohmann::json;

namespace digitalcurling::test {

bool EqualsShot(dc::moves::Shot const& s0, dc::moves::Shot const& s1);

void PlayerTestSaveLoad1(std::unique_ptr<dcp::IPlayer> player);
void PlayerTestSaveLoad2(std::unique_ptr<dcp::IPlayer> player);
void PlayerTestSaveLoad3(std::unique_ptr<dcp::IPlayer> player);

template<typename TStorage>
inline void PlayerTestSaveLoad4(std::unique_ptr<dcp::IPlayer> player)
{
    auto shot = dc::moves::Shot(0.1f, -1.57f, 2.f);

    // 1
    std::shared_ptr<dcp::IPlayerStorage> storage = player->CreateStorage();
    json j_storage = *std::dynamic_pointer_cast<TStorage>(storage);
    auto const shot0 = player->Play(shot);

    // 2
    auto storage2 = j_storage.get<TStorage>();
    auto player_copy = storage2.CreatePlayer();
    auto const shot1 = player_copy->Play(shot);

    EXPECT_TRUE(EqualsShot(shot0, shot1));
}

} // namespace digitalcurling::test
