#include <iostream>
#include <memory>
#include "common.hpp"

namespace digitalcurling::test {

bool EqualsShot(dc::moves::Shot const& s0, dc::moves::Shot const& s1)
{
    return s0.translational_velocity == s1.translational_velocity &&
           s0.angular_velocity == s1.angular_velocity &&
           s0.release_angle == s1.release_angle;
}

void PlayerTestSaveLoad1(std::unique_ptr<dcp::IPlayer> player)
{
    auto shot = dc::moves::Shot(0.1f, -1.57f, 2.f);
    player->Play(shot);

    auto storage = player->CreateStorage();
    auto const shot0 = player->Play(shot);

    player->Load(*storage);
    auto const shot1 = player->Play(shot);

    EXPECT_TRUE(EqualsShot(shot0, shot1));
}
void PlayerTestSaveLoad2(std::unique_ptr<dcp::IPlayer> player)
{
    auto shot = dc::moves::Shot(0.1f, -1.57f, 2.f);
    auto storage = player->CreateStorage();

    player->Play(shot);

    player->Save(*storage);
    auto const shot0 = player->Play(shot);

    player->Load(*storage);
    auto const shot1 = player->Play(shot);

    EXPECT_TRUE(EqualsShot(shot0, shot1));
}
void PlayerTestSaveLoad3(std::unique_ptr<dcp::IPlayer> player)
{
    auto shot = dc::moves::Shot(0.1f, -1.57f, 2.f);
    player->Play(shot);

    auto storage = player->CreateStorage();
    auto const shot0 = player->Play(shot);

    auto player_copy = storage->CreatePlayer();
    auto const shot1 = player_copy->Play(shot);

    EXPECT_TRUE(EqualsShot(shot0, shot1));
}

} // namespace digitalcurling::test
