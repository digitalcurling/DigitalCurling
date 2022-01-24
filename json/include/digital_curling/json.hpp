#ifndef DIGITAL_CURLING_JSON_JSON_HPP
#define DIGITAL_CURLING_JSON_JSON_HPP

#include "nlohmann/json.hpp"
#include "digital_curling/digital_curling.hpp"
#include "detail/json/polymorphic_conversion.hpp"


//////////////////////////////////////////////////////////////////
// std::optional
namespace nlohmann {
template <typename T>
struct adl_serializer<std::optional<T>> {
    static void to_json(json & j, std::optional<T> const& opt)
    {
        if (opt) {
            j = *opt;
        } else {
            j = nullptr;
        }
    }

    static void from_json(json const& j, std::optional<T> & opt)
    {
        if (j.is_null()) {
            opt = std::nullopt;
        } else {
            opt = j.get<T>();
        }
    }
};
} // namespace nlohmann


//////////////////////////////////////////////////////////////////
// digital_curling::Vector2
namespace digital_curling {
void to_json(nlohmann::json & j, Vector2 const& v);
void from_json(nlohmann::json const& j, Vector2 & v);
} // namespace digital_curling


//////////////////////////////////////////////////////////////////
// digital_curling::game::moves::Shot::Rotation
namespace digital_curling::game::moves {
NLOHMANN_JSON_SERIALIZE_ENUM(Shot::Rotation, {
    {Shot::Rotation::kCCW, "ccw"},
    {Shot::Rotation::kCW, "cw"},
})
} // namespace digital_curling::game::moves


//////////////////////////////////////////////////////////////////
// digital_curling::game::Move
namespace nlohmann {
template <>
struct adl_serializer<digital_curling::game::Move> {
    static void to_json(json & j, digital_curling::game::Move const& m);
    static void from_json(json const& j, digital_curling::game::Move & m);
};
} // namespace nlohmann


//////////////////////////////////////////////////////////////////
// digital_curling::game::random::IShotRandomizer
namespace digital_curling::game::random {
void to_json(nlohmann::json &, IShotRandomizer const&);
} // namespace digital_curling::game::random


//////////////////////////////////////////////////////////////////
// std::unique_ptr<digital_curling::game::random::IShotRandomizer>
namespace nlohmann {
template <>
struct adl_serializer<std::unique_ptr<digital_curling::game::random::IShotRandomizer>> {
    static std::unique_ptr<digital_curling::game::random::IShotRandomizer> from_json(json const&);
    static void to_json(json &, std::unique_ptr<digital_curling::game::random::IShotRandomizer> const&);
};
} // namespace nlohmann


//////////////////////////////////////////////////////////////////
// digital_curling::game::random::NormalDistShotRandomizer
namespace digital_curling::game::random {
void to_json(nlohmann::json &, NormalDistShotRandomizer const&);
void from_json(nlohmann::json const&, NormalDistShotRandomizer &);
} // namespace digital_curling::game::random


//////////////////////////////////////////////////////////////////
// digital_curling::game::random::RandomSeedNormalDistShotRandomizer
namespace digital_curling::game::random {
void to_json(nlohmann::json &, RandomSeedNormalDistShotRandomizer const&);
void from_json(nlohmann::json const&, RandomSeedNormalDistShotRandomizer &);
} // namespace digital_curling::game::random


//////////////////////////////////////////////////////////////////
// digital_curling::game::Team
namespace digital_curling::game {
NLOHMANN_JSON_SERIALIZE_ENUM(Team, {
    {Team::k0, 0},
    {Team::k1, 1},
    {Team::kInvalid, nullptr},
})
} // namespace digital_curling::game


//////////////////////////////////////////////////////////////////
// digital_curling::game::Result::Reason
namespace digital_curling::game {
NLOHMANN_JSON_SERIALIZE_ENUM(Result::Reason, {
    {Result::Reason::kScore, "score"},
    {Result::Reason::kConcede, "concede"},
    {Result::Reason::kTimeLimit, "time_limit"},
    {Result::Reason::kInvalid, nullptr},
})
} // namespace digital_curling::game


//////////////////////////////////////////////////////////////////
// digital_curling::game::Result
namespace digital_curling::game {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Result,
    win,
    reason
)
} // namespace digital_curling::game


//////////////////////////////////////////////////////////////////
// digital_curling::game::State
namespace digital_curling::game {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    State,
    stone_positions,
    scores,
    current_shot,
    current_end_first,
    current_end,
    extra_end_score,
    result
)
} // namespace digital_curling::game


//////////////////////////////////////////////////////////////////
// digital_curling::game::Setting
namespace digital_curling::game {
void to_json(nlohmann::json &, Setting const&);
void from_json(nlohmann::json const&, Setting &);
} // namespace digital_curling::game


//////////////////////////////////////////////////////////////////
// digital_curling::simulation::ISimulatorSetting
namespace digital_curling::simulation {
void to_json(nlohmann::json &, ISimulatorSetting const&);
} // namespace digital_curling::simulation


//////////////////////////////////////////////////////////////////
// std::unique_ptr<digital_curling::simulation::ISimulatorSetting>
namespace nlohmann {
template <>
struct adl_serializer<std::unique_ptr<digital_curling::simulation::ISimulatorSetting>> {
    static std::unique_ptr<digital_curling::simulation::ISimulatorSetting> from_json(json const&);
    static void to_json(json &, std::unique_ptr<digital_curling::simulation::ISimulatorSetting> const&);
};
} // namespace nlohmann


//////////////////////////////////////////////////////////////////
// digital_curling::simulation::SimulatorFCV1Setting
namespace digital_curling::simulation {
void to_json(nlohmann::json &, SimulatorFCV1Setting const&);
void from_json(nlohmann::json const&, SimulatorFCV1Setting &);
} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_JSON_JSON_HPP
