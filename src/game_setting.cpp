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

#include "digitalcurling3/detail/game_setting.hpp"


namespace digitalcurling3 {

void to_json(nlohmann::json& j, GameSetting const& v)
{
    j["max_end"] = v.max_end;
    j["sheet_width"] = v.sheet_width;
    j["five_rock_rule"] = v.five_rock_rule;
    j["thinking_time"]["team0"] = v.thinking_time[0];
    j["thinking_time"]["team1"] = v.thinking_time[1];
    j["extra_end_thinking_time"]["team0"] = v.extra_end_thinking_time[0];
    j["extra_end_thinking_time"]["team1"] = v.extra_end_thinking_time[1];
}

void from_json(nlohmann::json const& j, GameSetting& v)
{
    j.at("max_end").get_to(v.max_end);
    j.at("sheet_width").get_to(v.sheet_width);
    j.at("five_rock_rule").get_to(v.five_rock_rule);
    j.at("thinking_time").at("team0").get_to(v.thinking_time[0]);
    j.at("thinking_time").at("team1").get_to(v.thinking_time[1]);
    j.at("extra_end_thinking_time").at("team0").get_to(v.extra_end_thinking_time[0]);
    j.at("extra_end_thinking_time").at("team1").get_to(v.extra_end_thinking_time[1]);
}

} // namespace digitalcurling3
