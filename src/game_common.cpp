#include "digital_curling/detail/game/common.hpp"
#include "digital_curling/detail/game/random_seed_normal_dist_shot_randomizer.hpp"
#include "digital_curling/detail/game/normal_dist_shot_randomizer.hpp"

namespace digital_curling::game {

std::shared_ptr<IShotRandomizer> IShotRandomizer::GetDefault()
{
    static auto shot_randomizer = std::make_shared<RandomSeedNormalDistShotRandomizer>();
    return shot_randomizer;
}

void to_json(nlohmann::json & j, IShotRandomizer const& v)
{
    v.ToJson(j);
}

} // namespace digital_curling::game



namespace nlohmann {

void adl_serializer<digital_curling::game::Move>::to_json(json & j, digital_curling::game::Move const& m)
{
    std::visit(
        [&j](auto const& m) {
            j["type"] = std::decay_t<decltype(m)>::kType;
        },
        m);

    if (std::holds_alternative<digital_curling::game::Shot>(m)) {
        auto const& shot = std::get<digital_curling::game::Shot>(m);
        j["velocity"] = shot.velocity;
        j["rotation"] = shot.rotation;
    }
}

void adl_serializer<digital_curling::game::Move>::from_json(json const& j, digital_curling::game::Move & m)
{
    auto type = j.at("type").get<std::string>();
    if (type == digital_curling::game::Shot::kType) {
        digital_curling::game::Shot shot;
        j.at("velocity").get_to(shot.velocity);
        j.at("rotation").get_to(shot.rotation);
        m = std::move(shot);
    } else if (type == digital_curling::game::Concede::kType) {
        m = digital_curling::game::Concede();
    } else if (type == digital_curling::game::TimeLimit::kType) {
        m = digital_curling::game::TimeLimit();
    } else {
        throw std::runtime_error("Move type was not found.");
    }
}

std::unique_ptr<digital_curling::game::IShotRandomizer> adl_serializer<std::unique_ptr<digital_curling::game::IShotRandomizer>>::from_json(json const& j)
{
    auto type = j.at("type").get<std::string>();

    if (type == digital_curling::game::RandomSeedNormalDistShotRandomizer::kType) {
        return std::make_unique<digital_curling::game::RandomSeedNormalDistShotRandomizer>(j.get<digital_curling::game::RandomSeedNormalDistShotRandomizer>());
    } else if (type == digital_curling::game::NormalDistShotRandomizer::kType) {
        return std::make_unique<digital_curling::game::NormalDistShotRandomizer>(j.get<digital_curling::game::NormalDistShotRandomizer>());
    } else {
        throw std::runtime_error("no such type shot randomizer.");
    }
}

void adl_serializer<std::unique_ptr<digital_curling::game::IShotRandomizer>>::to_json(json & j, std::unique_ptr<digital_curling::game::IShotRandomizer> const& v)
{
    v->ToJson(j);
}

} // namespace nlohmann
