// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include "digitalcurling/common.hpp"

#include "player_normal_dist_factory.hpp"
#include "player_normal_dist.hpp"

namespace digitalcurling::players {

nlohmann::json PlayerNormalDistFactory::ToJson() const {
    nlohmann::json j;
    to_json(j, *this);
    return j;
}

std::unique_ptr<IPlayer> PlayerNormalDistFactory::CreatePlayer() const {
    return std::make_unique<PlayerNormalDist>(*this);
}
std::unique_ptr<IPlayerFactory> PlayerNormalDistFactory::Clone() const {
    return std::make_unique<PlayerNormalDistFactory>(*this);
}

// json
void to_json(nlohmann::json & j, PlayerNormalDistFactory const& v) {
    j["type"] = DIGITALCURLING_PLUGIN_NAME;
    j["gender"] = v.gender;
    j["cw"] = v.cw;
    j["ccw"] = v.ccw;
    j["seed"] = v.seed;
}
void from_json(nlohmann::json const& j, PlayerNormalDistFactory & v) {
    j.at("gender").get_to(v.gender);

    if (j.contains("cw") && j.contains("ccw")) {
        j.at("cw").get_to(v.cw);
        j.at("ccw").get_to(v.ccw);
    } else {
        float max_speed, stddev_speed, stddev_angle;
        j.at("max_speed").get_to(max_speed);
        j.at("stddev_speed").get_to(stddev_speed);
        j.at("stddev_angle").get_to(stddev_angle);
        v.cw = {max_speed, stddev_speed, stddev_angle};
        v.ccw = {max_speed, stddev_speed, stddev_angle};
    }

    if (j.contains("seed")) {
        j.at("seed").get_to(v.seed);
    } else {
        v.seed = std::nullopt;
    }
}

} // namespace digitalcurling::players
