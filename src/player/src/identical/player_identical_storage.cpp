// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <memory>
#include <nlohmann/json.hpp>
#include "digitalcurling/common.hpp"

#include "player_identical.hpp"
#include "player_identical_storage.hpp"

namespace digitalcurling::players {

nlohmann::json PlayerIdenticalStorage::ToJson() const {
    nlohmann::json j;
    to_json(j, *this);
    return j;
}

std::unique_ptr<IPlayer> PlayerIdenticalStorage::CreatePlayer() const {
    return std::make_unique<PlayerIdentical>(*this);
}

// json
void to_json(nlohmann::json & j, PlayerIdenticalStorage const& v) {
    j["type"] = DIGITALCURLING_PLUGIN_NAME;
    j["gender"] = v.gender;
}

void from_json(nlohmann::json const& j, PlayerIdenticalStorage & v) {
    j.at("gender").get_to(v.gender);
}

} // namespace digitalcurling::players
