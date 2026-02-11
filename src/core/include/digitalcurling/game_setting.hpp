// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief GameSetting を定義

#pragma once

#include <cstdint>
#include <chrono>
#include <nlohmann/json.hpp>
#include "digitalcurling/common.hpp"
#include "digitalcurling/team.hpp"

namespace digitalcurling {


/// @brief 試合設定
struct GameSetting {

    /// @brief 延長戦を含まないエンド数
    std::uint8_t max_end = 10;

    /// @brief アイスシートの横幅
    float sheet_width = 4.75f;

    /// @brief 各チームの全エンド(延長エンドを除く)の思考時間
    /// @sa extra_end_thinking_time 延長エンドの思考時間
    TeamValue<std::chrono::milliseconds> thinking_time;

    /// @brief 延長エンドの思考時間
    ///
    /// 残り思考時間は各延長エンドが始まる際にこの値で上書きされます。
    ///
    /// @sa thinking_time 延長ではないエンドの思考時間
    TeamValue<std::chrono::milliseconds> extra_end_thinking_time;
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameSetting, max_end, sheet_width, thinking_time, extra_end_thinking_time)
/// @endcond

} // namespace digitalcurling
