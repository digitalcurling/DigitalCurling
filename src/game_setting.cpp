#include "digital_curling/detail/game_setting.hpp"


namespace digital_curling {

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

} // namespace digital_curling
