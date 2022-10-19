// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "digitalcurling3/detail/move.hpp"

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

void adl_serializer<digitalcurling3::Move>::to_json(json & j, digitalcurling3::Move const& m)
{
    std::visit(Overloaded{
        [&j] (digitalcurling3::moves::Shot const& shot) {
            j["type"] = kMoveTypeShot;
            j["velocity"] = shot.velocity;
            j["rotation"] = shot.rotation;
        },
        [&j] (digitalcurling3::moves::Concede const&) {
            j["type"] = kMoveTypeConcede;
        }},
        m);
}

void adl_serializer<digitalcurling3::Move>::from_json(json const& j, digitalcurling3::Move & m)
{
    auto type = j.at("type").get<std::string>();
    if (type == kMoveTypeShot) {
        digitalcurling3::moves::Shot shot;
        j.at("velocity").get_to(shot.velocity);
        j.at("rotation").get_to(shot.rotation);
        m = std::move(shot);
    } else if (type == kMoveTypeConcede) {
        m = digitalcurling3::moves::Concede();
    } else {
        throw std::runtime_error("Move type was not found.");
    }
}

} // namespace nlohmann
