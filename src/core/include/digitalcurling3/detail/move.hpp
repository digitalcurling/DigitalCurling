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

/// \file
/// \brief Move を定義します

#ifndef DIGITALCURLING3_MOVE_HPP
#define DIGITALCURLING3_MOVE_HPP

#include <variant>
#include "moves/shot.hpp"
#include "moves/concede.hpp"

namespace digitalcurling3 {



/// \brief ターンごとにチームが採る行動
///
/// \sa moves::Shot, moves::Concede
using Move = std::variant<moves::Shot, moves::Concede>;



} // namespace digitalcurling3


/// \cond Doxygen_Suppress
// json
namespace nlohmann {

template <>
struct adl_serializer<digitalcurling3::Move> {
    static void to_json(json & j, digitalcurling3::Move const& m) {
        std::visit(Overloaded{
            [&j] (digitalcurling3::moves::Shot const& shot) {
                nlohmann::json temp;
                digitalcurling3::moves::to_json(temp, shot);

                j["type"] = digitalcurling3::moves::Shot::kTypeName;
                j.update(temp);
            },
            [&j] (digitalcurling3::moves::Concede const&) {
                j["type"] = digitalcurling3::moves::Concede::kTypeName;
            }},
            m
        );
    }

    static void from_json(json const& j, digitalcurling3::Move & m) {
        auto type = j.at("type").get<std::string>();
        if (type == digitalcurling3::moves::Shot::kTypeName) {
            m = j.get<digitalcurling3::moves::Shot>();
        } else if (type == digitalcurling3::moves::Concede::kTypeName) {
            m = digitalcurling3::moves::Concede();
        } else {
            throw std::runtime_error("Move type was not found.");
        }
    }
};

} // namespace nlohmann
/// \endcond


#endif // DIGITALCURLING3_MOVE_HPP
