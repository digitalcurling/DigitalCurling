#include "common.hpp"

#include <vector>
#include <memory>

namespace dc = digitalcurling3;
using nlohmann::json;

namespace {

std::vector<std::unique_ptr<dc::IPlayer>> CreatePlayers()
{
    std::vector<std::unique_ptr<dc::IPlayer>> players;

    // identical
    {
        dc::players::IdenticalPlayerFactory factory;
        players.emplace_back(factory.CreatePlayer());
    }

    // normal_dist
    {
        dc::players::NormalDistPlayerFactory factory;
        players.emplace_back(factory.CreatePlayer());
    }

    return players;
}

bool EqualsVector2(dc::Vector2 const& v0, dc::Vector2 const& v1)
{
    return v0.x == v1.x && v0.y == v1.y;
}

void PrintVector2(dc::Vector2 const& v)
{
    std::cout << "x: " << v.x << ", y: " << v.y << std::endl;
}

} // unnamed namespace

TEST(Players, SaveLoad)
{
    auto players = CreatePlayers();

    for (auto & player : players) {

        player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        auto storage = player->CreateStorage();
        auto const shot0 = player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        player->Load(*storage);
        auto const shot1 = player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        // PrintVector2(shot0);
        // PrintVector2(shot1);

        EXPECT_TRUE(EqualsVector2(shot0.velocity, shot1.velocity)) << player->GetPlayerId();
    }
}

TEST(Players, SaveLoad2)
{
    auto players = CreatePlayers();

    for (auto & player : players) {
        auto storage = player->CreateStorage();

        player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        player->Save(*storage);
        auto const shot0 = player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        player->Load(*storage);
        auto const shot1 = player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        // PrintVector2(shot0);
        // PrintVector2(shot1);

        EXPECT_TRUE(EqualsVector2(shot0.velocity, shot1.velocity)) << player->GetPlayerId();
    }
}

TEST(Players, SaveLoad3)
{
    auto players = CreatePlayers();

    for (auto & player : players) {
        player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        auto storage = player->CreateStorage();
        auto const shot0 = player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        auto player_copy = storage->CreatePlayer();
        auto const shot1 = player_copy->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        // PrintVector2(shot0);
        // PrintVector2(shot1);

        EXPECT_TRUE(EqualsVector2(shot0.velocity, shot1.velocity)) << player->GetPlayerId();
    }
}

TEST(Players, SaveLoad4)
{
    auto players = CreatePlayers();

    for (auto & player : players) {
        player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        auto storage = player->CreateStorage();
        json j_storage = *storage;
        auto const shot0 = player->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        auto storage2 = j_storage.get<std::unique_ptr<dc::IPlayerStorage>>();
        auto player_copy = storage2->CreatePlayer();
        auto const shot1 = player_copy->Play({ dc::Vector2(0.1f, 2.f), dc::moves::Shot::Rotation::kCCW });

        // PrintVector2(shot0);
        // PrintVector2(shot1);

        EXPECT_TRUE(EqualsVector2(shot0.velocity, shot1.velocity)) << player->GetPlayerId();
    }
}
