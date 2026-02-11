// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <memory>
#include <stdexcept>
#include <string>
#include "digitalcurling/common.hpp"

#include "player_normal_dist_storage.hpp"
#include "player_normal_dist.hpp"

namespace digitalcurling::players {

nlohmann::json PlayerNormalDistStorage::ToJson() const {
    nlohmann::json j;
    to_json(j, *this);
    return j;
}

std::unique_ptr<IPlayer> PlayerNormalDistStorage::CreatePlayer() const {
    return std::make_unique<PlayerNormalDist>(*this);
}

// json
void to_json(nlohmann::json & j, PlayerNormalDistStorage const& v) {
    j["type"] = DIGITALCURLING_PLUGIN_NAME;
    j["factory"] = v.factory;
    j["engine_data"] = v.engine_data;
    j["ccw_data"] = v.ccw_data;
    j["cw_data"] = v.cw_data;
}
void from_json(nlohmann::json const& j, PlayerNormalDistStorage & v) {
    j.at("factory").get_to(v.factory);
    j.at("engine_data").get_to(v.engine_data);

    if (j.contains("ccw_data") && j.contains("cw_data")) {
        j.at("ccw_data").get_to(v.ccw_data);
        j.at("cw_data").get_to(v.cw_data);
    } else if (j.contains("speed_dist_data") && j.contains("angle_dist_data")) {
        std::string speed_dist, angle_dist;
        j.at("speed_dist_data").get_to(speed_dist);
        j.at("angle_dist_data").get_to(angle_dist);
        v.ccw_data = {speed_dist, angle_dist};
        v.cw_data = {speed_dist, angle_dist};
    } else {
        throw std::runtime_error("Invalid JSON format: missing ccw_data or cw_data");
    }
}

} // namespace digitalcurling::players
