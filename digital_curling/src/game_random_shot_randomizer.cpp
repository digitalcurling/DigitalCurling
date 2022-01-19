#include "digital_curling/detail/game/random/shot_randomizer.hpp"
#include "digital_curling/detail/game/random/normal_dist_shot_randomizer.hpp"
#include "digital_curling/detail/game/random/random_seed_normal_dist_shot_randomizer.hpp"

namespace digital_curling::game::random {

std::shared_ptr<IShotRandomizer> IShotRandomizer::GetDefault()
{
    static auto shot_randomizer = std::make_shared<RandomSeedNormalDistShotRandomizer>();
    return shot_randomizer;
}

void to_json(nlohmann::json & j, IShotRandomizer const& v)
{
    v.ToJson(j);
}

} // namespace digital_curling::game::random

namespace nlohmann {

std::unique_ptr<digital_curling::game::random::IShotRandomizer> adl_serializer<std::unique_ptr<digital_curling::game::random::IShotRandomizer>>::from_json(json const& j)
{
    auto type = j.at("type").get<std::string>();

    if (type == digital_curling::game::random::RandomSeedNormalDistShotRandomizer::kType) {
        return std::make_unique<digital_curling::game::random::RandomSeedNormalDistShotRandomizer>(j.get<digital_curling::game::random::RandomSeedNormalDistShotRandomizer>());
    } else if (type == digital_curling::game::random::NormalDistShotRandomizer::kType) {
        return std::make_unique<digital_curling::game::random::NormalDistShotRandomizer>(j.get<digital_curling::game::random::NormalDistShotRandomizer>());
    } else {
        throw std::runtime_error("no such type shot randomizer.");
    }
}

void adl_serializer<std::unique_ptr<digital_curling::game::random::IShotRandomizer>>::to_json(json & j, std::unique_ptr<digital_curling::game::random::IShotRandomizer> const& v)
{
    v->ToJson(j);
}

} // namespace nlohmann
