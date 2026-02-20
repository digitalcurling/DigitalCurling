// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include "simulator_fcv1.hpp"
#include "digitalcurling/plugins/simulator_plugin_export.hpp"

DIGITALCURLING_EXPORT_INVERTIBLE_SIMULATOR_PLUGIN(
    digitalcurling::simulators::SimulatorFCV1Factory,
    digitalcurling::simulators::SimulatorFCV1Storage,
    digitalcurling::simulators::SimulatorFCV1,
    &digitalcurling::simulators::SimulatorFCV1::CalculateShot
)
