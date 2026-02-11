// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief Move を定義

#pragma once

#include <variant>
#include "digitalcurling/moves/shot.hpp"
#include "digitalcurling/moves/concede.hpp"

namespace digitalcurling {


/// @brief ターンごとにチームが採る行動
/// @sa moves::Shot, moves::Concede
using Move = std::variant<moves::Shot, moves::Concede>;

} // namespace digitalcurling


/// @cond Doxygen_Suppress
// json
namespace nlohmann {

template <>
struct adl_serializer<digitalcurling::Move> {
    static void to_json(json & j, digitalcurling::Move const& m) {
        std::visit(Overloaded{
            [&j] (digitalcurling::moves::Shot const& shot) {
                nlohmann::json temp;
                digitalcurling::moves::to_json(temp, shot);

                j["type"] = digitalcurling::moves::Shot::kTypeName;
                j.update(temp);
            },
            [&j] (digitalcurling::moves::Concede const&) {
                j["type"] = digitalcurling::moves::Concede::kTypeName;
            }},
            m
        );
    }

    static void from_json(json const& j, digitalcurling::Move & m) {
        auto type = j.at("type").get<std::string>();
        if (type == digitalcurling::moves::Shot::kTypeName) {
            m = j.get<digitalcurling::moves::Shot>();
        } else if (type == digitalcurling::moves::Concede::kTypeName) {
            m = digitalcurling::moves::Concede();
        } else {
            throw std::runtime_error("Move type was not found.");
        }
    }
};

} // namespace nlohmann
/// @endcond
