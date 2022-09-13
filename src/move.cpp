#include "digital_curling/detail/move.hpp"

// json
namespace {

constexpr char kMoveTypeShot[] = "shot";
constexpr char kMoveTypeConcede[] = "concede";

// overloadedトリック用ヘルパークラス
// 参考: https://dev.to/tmr232/that-overloaded-trick-overloading-lambdas-in-c17
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...)->Overloaded<Ts...>;

} // unnamed namespace

namespace nlohmann {

void adl_serializer<digital_curling::Move>::to_json(json & j, digital_curling::Move const& m)
{
    std::visit(Overloaded{
        [&j] (digital_curling::moves::Shot const& shot) {
            j["type"] = kMoveTypeShot;
            j["velocity"] = shot.velocity;
            j["rotation"] = shot.rotation;
        },
        [&j] (digital_curling::moves::Concede const&) {
            j["type"] = kMoveTypeConcede;
        }},
        m);
}

void adl_serializer<digital_curling::Move>::from_json(json const& j, digital_curling::Move & m)
{
    auto type = j.at("type").get<std::string>();
    if (type == kMoveTypeShot) {
        digital_curling::moves::Shot shot;
        j.at("velocity").get_to(shot.velocity);
        j.at("rotation").get_to(shot.rotation);
        m = std::move(shot);
    } else if (type == kMoveTypeConcede) {
        m = digital_curling::moves::Concede();
    } else {
        throw std::runtime_error("Move type was not found.");
    }
}

} // namespace nlohmann
