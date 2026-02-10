// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief Move を定義します

#pragma once

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
