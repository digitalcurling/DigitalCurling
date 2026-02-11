// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief GameSetting を定義

#pragma once

#include <cstdint>
#include <chrono>
#include <array>
#include "common.hpp"

namespace digitalcurling {


/// @brief 試合設定
struct GameSetting {

    /// @brief 延長戦を含まないエンド数
    std::uint8_t max_end = 10;

    /// @brief アイスシートの横幅
    float sheet_width = 4.75f;

    /// @brief ファイブロックルールを適用するか
    ///
    /// ファイブロックルールを適用する場合 `true` を指定します。
    bool five_rock_rule = true;

    /// @brief 各チームの全エンド(延長エンドを除く)の思考時間
    /// @sa extra_end_thinking_time 延長エンドの思考時間
    std::array<std::chrono::milliseconds, 2> thinking_time;

    /// @brief 延長エンドの思考時間
    ///
    /// 残り思考時間は各延長エンドが始まる際にこの値で上書きされます。
    ///
    /// @sa thinking_time 延長ではないエンドの思考時間
    std::array<std::chrono::milliseconds, 2> extra_end_thinking_time;
};


/// @cond Doxygen_Suppress
// json
inline void to_json(nlohmann::json& j, GameSetting const& v) {
    j["max_end"] = v.max_end;
    j["sheet_width"] = v.sheet_width;
    j["five_rock_rule"] = v.five_rock_rule;
    j["thinking_time"]["team0"] = v.thinking_time[0];
    j["thinking_time"]["team1"] = v.thinking_time[1];
    j["extra_end_thinking_time"]["team0"] = v.extra_end_thinking_time[0];
    j["extra_end_thinking_time"]["team1"] = v.extra_end_thinking_time[1];
}

inline void from_json(nlohmann::json const& j, GameSetting& v) {
    j.at("max_end").get_to(v.max_end);
    j.at("sheet_width").get_to(v.sheet_width);
    j.at("five_rock_rule").get_to(v.five_rock_rule);
    j.at("thinking_time").at("team0").get_to(v.thinking_time[0]);
    j.at("thinking_time").at("team1").get_to(v.thinking_time[1]);
    j.at("extra_end_thinking_time").at("team0").get_to(v.extra_end_thinking_time[0]);
    j.at("extra_end_thinking_time").at("team1").get_to(v.extra_end_thinking_time[1]);
}
/// @endcond

} // namespace digitalcurling
