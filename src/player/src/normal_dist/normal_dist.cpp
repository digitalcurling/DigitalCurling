// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "player_normal_dist.hpp"
#include "digitalcurling/plugins/player_plugin_export.hpp"

DIGITALCURLING_EXPORT_PLAYER_PLUGIN(
    digitalcurling::players::PlayerNormalDistFactory,
    digitalcurling::players::PlayerNormalDistStorage,
    digitalcurling::players::PlayerNormalDist
)
