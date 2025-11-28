// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file
/// \brief GameSetting を定義します

#ifndef DIGITALCURLING3_GAME_SETTING_HPP
#define DIGITALCURLING3_GAME_SETTING_HPP

#include <cstdint>
#include <chrono>
#include <array>
#include "common.hpp"

namespace digitalcurling3 {

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

} // namespace digitalcurling3

#endif // DIGITALCURLING3_GAME_SETTING_HPP
