#include <memory>
#include <string>
#include "common.hpp"
#include "../src/identical/player_identical.hpp"
#include "../src/identical/player_identical_factory.hpp"

namespace dct = digitalcurling::test;

TEST(PlayerIdentical, SaveLoad1)
{
    dcp::PlayerIdenticalFactory factory;
    dct::PlayerTestSaveLoad1(factory.CreatePlayer());
}

TEST(PlayerIdentical, SaveLoad2)
{
    dcp::PlayerIdenticalFactory factory;
    dct::PlayerTestSaveLoad2(factory.CreatePlayer());
}

TEST(PlayerIdentical, SaveLoad3)
{
    dcp::PlayerIdenticalFactory factory;
    dct::PlayerTestSaveLoad3(factory.CreatePlayer());
}

TEST(PlayerIdentical, SaveLoad4)
{
    dcp::PlayerIdenticalFactory factory;
    dct::PlayerTestSaveLoad4<dcp::PlayerIdenticalStorage>(factory.CreatePlayer());
}


TEST(PlayerIdentical, FactoryToJson)
{
    auto v_identical = std::make_unique<dcp::PlayerIdenticalFactory>();

    json const j_identical = *v_identical.get();
    EXPECT_EQ(j_identical.at("type").get<std::string>(), "identical");
}

TEST(PlayerIdentical, FactoryFromJson)
{
    json const j_identical{
        { "type", "identical" },
        { "gender", "male" }
    };

    dcp::PlayerIdenticalFactory v_identical;
    EXPECT_NO_THROW(v_identical = j_identical.get<dcp::PlayerIdenticalFactory>());
    EXPECT_EQ(v_identical.GetGender(), dcp::Gender::kMale);
}
