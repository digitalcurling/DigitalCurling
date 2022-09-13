/// \file
/// \brief GameSetting を定義します

#ifndef DIGITAL_CURLING_GAME_SETTING_HPP
#define DIGITAL_CURLING_GAME_SETTING_HPP

#include <cstdint>
#include <chrono>
#include <array>
#include "json/common.hpp"

namespace digital_curling {

/// \brief 試合設定
struct GameSetting {

    /// \brief 延長戦を含まないエンド数．
    std::uint8_t max_end = 10;

    /// \brief アイスシートの横幅．
    float sheet_width = 4.75f;

    /// \brief ファイブロックルールを適用するか？
    ///
    /// ファイブロックルールを適用する場合 \c true を指定します．
    bool five_rock_rule = true;

    /// \brief 各チームの全エンド(延長エンドを除く)の思考時間です
    ///
    /// \sa extra_end_thinking_time 延長エンドの思考時間
    std::array<std::chrono::milliseconds, 2> thinking_time;

    /// \brief 延長エンドの思考時間です．
    ///
    /// 残り思考時間は各延長エンドが始まる際にこの値で上書きされます．
    ///
    /// \sa thinking_time 延長ではないエンドの思考時間
    std::array<std::chrono::milliseconds, 2> extra_end_thinking_time;
};


/// \cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, GameSetting const&);
void from_json(nlohmann::json const&, GameSetting &);
/// \endcond

} // namespace digital_curling

#endif // DIGITAL_CURLING_GAME_SETTING_HPP
