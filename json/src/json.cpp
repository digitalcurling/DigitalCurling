#include "digital_curling/json.hpp"

namespace {

// overloadedトリック用ヘルパークラス
// 参考: https://dev.to/tmr232/that-overloaded-trick-overloading-lambdas-in-c17
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...)->Overloaded<Ts...>;

constexpr char kMoveTypeShot[] = "shot";
constexpr char kMoveTypeConcede[] = "concede";
constexpr char kMoveTypeTimeLimit[] = "time_limit";

constexpr char kShotRandomizerTypeNormalDist[] = "normal_dist";
constexpr char kShotRandomizerTypeRandomSeedNormalDist[] = "random_seed_normal_dist";

constexpr char kSimulatorSettingTypeFCV1[] = "fcv1";
} // unnamed namespace



//////////////////////////////////////////////////////////////////
// digital_curling::Vector2
namespace digital_curling {

void to_json(nlohmann::json & j, Vector2 const& v)
{
    j[0] = v.x;
    j[1] = v.y;
}

void from_json(nlohmann::json const& j, Vector2 & v)
{
    v.x = j.at(0);
    v.y = j.at(1);
}

} // namespace digital_curling


//////////////////////////////////////////////////////////////////
// digital_curling::game::Move
namespace nlohmann {

void adl_serializer<digital_curling::game::Move>::to_json(json & j, digital_curling::game::Move const& m)
{
    std::visit(Overloaded{
        [&j] (digital_curling::game::moves::Shot const& shot) {
            j["type"] = kMoveTypeShot;
            j["velocity"] = shot.velocity;
            j["rotation"] = shot.rotation;
        },
        [&j] (digital_curling::game::moves::Concede const&) {
            j["type"] = kMoveTypeConcede;
        },
        [&j] (digital_curling::game::moves::TimeLimit const&) {
            j["type"] = kMoveTypeTimeLimit;
        }},
        m);
}

void adl_serializer<digital_curling::game::Move>::from_json(json const& j, digital_curling::game::Move & m)
{
    auto type = j.at("type").get<std::string>();
    if (type == kMoveTypeShot) {
        digital_curling::game::moves::Shot shot;
        j.at("velocity").get_to(shot.velocity);
        j.at("rotation").get_to(shot.rotation);
        m = std::move(shot);
    } else if (type == kMoveTypeConcede) {
        m = digital_curling::game::moves::Concede();
    } else if (type == kMoveTypeTimeLimit) {
        m = digital_curling::game::moves::TimeLimit();
    } else {
        throw std::runtime_error("Move type was not found.");
    }
}

} // namespace nlohmann


//////////////////////////////////////////////////////////////////
// digital_curling::game::random::NormalDistShotRandomizer
namespace digital_curling::game::random {

void to_json(nlohmann::json & j, NormalDistShotRandomizer const& v)
{
    j["type"] = kShotRandomizerTypeNormalDist;
    j["stddev_speed"] = v.stddev_speed;
    j["stddev_angle"] = v.stddev_angle;
    j["seed"] = v.seed;
}

void from_json(nlohmann::json const& j, NormalDistShotRandomizer & v)
{
    j.at("stddev_speed").get_to(v.stddev_speed);
    j.at("stddev_angle").get_to(v.stddev_angle);
    j.at("seed").get_to(v.seed);
}

DIGITAL_CURLING_REGISTER_POLYMORPHIC_JSON_CONVERSION(IShotRandomizer, NormalDistShotRandomizer, kShotRandomizerTypeNormalDist)

} // namespace digital_curling::game::random


//////////////////////////////////////////////////////////////////
// digital_curling::game::random::RandomSeedNormalDistShotRandomizer
namespace digital_curling::game::random {

void to_json(nlohmann::json & j, RandomSeedNormalDistShotRandomizer const& v)
{
    j["type"] = kShotRandomizerTypeRandomSeedNormalDist;
    j["stddev_speed"] = v.stddev_speed;
    j["stddev_angle"] = v.stddev_angle;
}

void from_json(nlohmann::json const& j, RandomSeedNormalDistShotRandomizer & v)
{
    j.at("stddev_speed").get_to(v.stddev_speed);
    j.at("stddev_angle").get_to(v.stddev_angle);
}

DIGITAL_CURLING_REGISTER_POLYMORPHIC_JSON_CONVERSION(IShotRandomizer, RandomSeedNormalDistShotRandomizer, kShotRandomizerTypeRandomSeedNormalDist)

} // namespace digital_curling::game::random


//////////////////////////////////////////////////////////////////
// digital_curling::game::random::IShotRandomizer
namespace digital_curling::game::random {

void to_json(nlohmann::json & j, IShotRandomizer const& v)
{
    detail::PolymorphicJsonConverter<IShotRandomizer>::ToJson(j, v);
}

} // namespace digital_curling::game::random


//////////////////////////////////////////////////////////////////
// std::unique_ptr<digital_curling::game::random::IShotRandomizer>
namespace nlohmann {

std::unique_ptr<digital_curling::game::random::IShotRandomizer> adl_serializer<std::unique_ptr<digital_curling::game::random::IShotRandomizer>>::from_json(json const& j)
{
    return digital_curling::detail::PolymorphicJsonConverter<digital_curling::game::random::IShotRandomizer>::FromJson(j);
}

void adl_serializer<std::unique_ptr<digital_curling::game::random::IShotRandomizer>>::to_json(json & j, std::unique_ptr<digital_curling::game::random::IShotRandomizer> const& v)
{
    j = *v;
}

} // namespace nlohmann


//////////////////////////////////////////////////////////////////
// digital_curling::game::Setting
namespace digital_curling::game {

void to_json(nlohmann::json & j, Setting const& v)
{
    j["randomize_initial_shot_velocity"] = v.randomize_initial_shot_velocity;
    j["end"] = v.end;
    j["sheet_width"] = v.sheet_width;
    j["five_rock_rule"] = v.five_rock_rule;
    j["max_shot_speed"] = v.max_shot_speed;
    j["shot_randomizer"] = *v.shot_randomizer;
}

void from_json(nlohmann::json const& j, Setting & v)
{
    j.at("randomize_initial_shot_velocity").get_to(v.randomize_initial_shot_velocity);
    j.at("end").get_to(v.end);
    j.at("sheet_width").get_to(v.sheet_width);
    j.at("five_rock_rule").get_to(v.five_rock_rule);
    j.at("max_shot_speed").get_to(v.max_shot_speed);
    v.shot_randomizer = j.at("shot_randomizer").get<std::unique_ptr<random::IShotRandomizer>>();
}

} // namespace digital_curling::game


//////////////////////////////////////////////////////////////////
// digital_curling::simulation::ISimulatorSetting
namespace digital_curling::simulation {

void to_json(nlohmann::json & j, ISimulatorSetting const& v)
{
    detail::PolymorphicJsonConverter<ISimulatorSetting>::ToJson(j, v);
}

} // namespace digital_curling::simulation


//////////////////////////////////////////////////////////////////
// std::unique_ptr<digital_curling::simulation::ISimulatorSetting>
namespace nlohmann {

std::unique_ptr<digital_curling::simulation::ISimulatorSetting> adl_serializer<std::unique_ptr<digital_curling::simulation::ISimulatorSetting>>::from_json(json const& j)
{
    return digital_curling::detail::PolymorphicJsonConverter<digital_curling::simulation::ISimulatorSetting>::FromJson(j);
}

void adl_serializer<std::unique_ptr<digital_curling::simulation::ISimulatorSetting>>::to_json(json & j, std::unique_ptr<digital_curling::simulation::ISimulatorSetting> const& v)
{
    j = *v;
}

} // namespace nlohmann


//////////////////////////////////////////////////////////////////
// digital_curling::simulation::SimulatorFCV1Setting
namespace digital_curling::simulation {

void to_json(nlohmann::json & j, const SimulatorFCV1Setting & setting)
{
    j["type"] = kSimulatorSettingTypeFCV1;
    j["seconds_per_frame"] = setting.seconds_per_frame;
}

void from_json(nlohmann::json const& j, SimulatorFCV1Setting & setting)
{
    j.at("seconds_per_frame").get_to(setting.seconds_per_frame);
}

DIGITAL_CURLING_REGISTER_POLYMORPHIC_JSON_CONVERSION(ISimulatorSetting, SimulatorFCV1Setting, kSimulatorSettingTypeFCV1)

} // namespace digital_curling::simulation
