// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <memory>
#include <nlohmann/json.hpp>
#include "digitalcurling/common.hpp"

#include "player_identical_factory.hpp"
#include "player_identical.hpp"

namespace digitalcurling::players {

nlohmann::json PlayerIdenticalFactory::ToJson() const {
    nlohmann::json j;
    to_json(j, *this);
    return j;
}

std::unique_ptr<IPlayer> PlayerIdenticalFactory::CreatePlayer() const {
    return std::make_unique<PlayerIdentical>(*this);
}

std::unique_ptr<IPlayerFactory> PlayerIdenticalFactory::Clone() const {
    return std::make_unique<PlayerIdenticalFactory>(*this);
}

// json
void to_json(nlohmann::json & j, PlayerIdenticalFactory const& v) {
    j["type"] = DIGITALCURLING_PLUGIN_NAME;
    j["gender"] = v.gender;
}

void from_json(nlohmann::json const& j, PlayerIdenticalFactory & v) {
    j.at("gender").get_to(v.gender);
}

} // namespace digitalcurling::players
