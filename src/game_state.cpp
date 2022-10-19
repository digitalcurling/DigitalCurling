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

#include "digitalcurling3/detail/game_state.hpp"

#include <stdexcept>
#include "digitalcurling3/detail/coordinate.hpp"

namespace digitalcurling3 {

GameState::GameState()
    : end(0)
    , shot(0)
    , hammer(Team::k1)
    , stones{{ {}, {} }}
    , scores{{ {}, {} }}
    , extra_end_score{{}}
    , thinking_time_remaining{{}}
    , game_result()
{}

GameState::GameState(GameSetting const& setting)
    : end(0)
    , shot(0)
    , hammer(Team::k1)
    , stones{{ {}, {} }}
    , scores{{ std::vector<std::optional<std::uint8_t>>(setting.max_end),
               std::vector<std::optional<std::uint8_t>>(setting.max_end) }}
    , extra_end_score{{}}
    , thinking_time_remaining{{ setting.thinking_time[0], setting.thinking_time[1] }}
    , game_result()
{}

std::uint32_t GameState::GetTotalScore(Team team) const
{
    if (team == Team::kInvalid) {
        throw std::invalid_argument("team");
    }

    std::uint32_t score_sum = 0;

    for (auto const& score : scores[size_t(team)]) {
        if (score) {
            score_sum += *score;
        }
    }

    if (auto const& exscore = extra_end_score[size_t(team)]; exscore) {
        score_sum += *exscore;
    }

    return score_sum;
}

Team GameState::GetNextTeam() const
{
    if (IsGameOver()) {
        return Team::kInvalid;
    }

    switch (hammer) {
        case Team::k0:
            if (shot % 2 == 0) {
                return Team::k1;
            } else {
                return Team::k0;
            }
            break;  // 到達しない

        case Team::k1:
            if (shot % 2 == 0) {
                return Team::k0;
            } else {
                return Team::k1;
            }
            break;  // 到達しない

        default:
            throw std::logic_error("invalid state");
    }

    return Team::k0;  // 到達しない
}


std::pair<Team, size_t> GameState::StonesIndexFromAllStonesIndex(size_t all_stones_index)
{
    assert(all_stones_index < kShotPerEnd);

    return {
        (all_stones_index < kShotPerEnd / 2) ? Team::k0 : Team::k1,
        all_stones_index % (kShotPerEnd / 2)
    };
}

size_t GameState::StonesIndexToAllStonesIndex(Team team, size_t team_stone_index)
{
    return static_cast<size_t>(team) * GameState::kShotPerEnd / 2 + team_stone_index;
}

GameState::Stones GameState::StonesFromAllStones(ISimulator::AllStones const& all_stones, std::uint8_t end)
{
    auto const shot_side = coordinate::GetShotSide(end);

    GameState::Stones state_stones;
    for (std::uint8_t i = 0; i < GameState::kShotPerEnd; ++i) {
        auto const [team, team_stone_idx] = StonesIndexFromAllStonesIndex(i);
        auto& state_stone = state_stones[static_cast<size_t>(team)][team_stone_idx];

        if (all_stones[i]) {
            state_stone.emplace(
                coordinate::TransformPosition(all_stones[i]->position, coordinate::Id::kSimulation, shot_side),
                coordinate::TransformAngle(all_stones[i]->angle, coordinate::Id::kSimulation, shot_side));
        } else {
            state_stone = std::nullopt;
        }
    }
    return state_stones;
}

ISimulator::AllStones GameState::StonesToAllStones(GameState::Stones const& stones, std::uint8_t end)
{
    auto const shot_side = coordinate::GetShotSide(end);

    constexpr std::array<Team, 2> kTeams{{ Team::k0, Team::k1 }};
    ISimulator::AllStones all_stones;
    for (Team team : kTeams) {
        for (size_t team_stone_idx = 0; team_stone_idx < GameState::kShotPerEnd / 2; ++team_stone_idx) {
            auto const& stone = stones[static_cast<size_t>(team)][team_stone_idx];
            auto const idx = StonesIndexToAllStonesIndex(team, team_stone_idx);
            if (stone) {
                all_stones[idx].emplace(
                    coordinate::TransformPosition(stone->position, shot_side, coordinate::Id::kSimulation),
                    coordinate::TransformAngle(stone->angle, shot_side, coordinate::Id::kSimulation),
                    Vector2(0.f, 0.f),
                    0.f);
            } else {
                all_stones[idx] = std::nullopt;
            }
        }
    }
    return all_stones;
}


// json

void to_json(nlohmann::json& j, GameState const& v)
{
    j["end"] = v.end;
    j["shot"] = v.shot;
    j["hammer"] = v.hammer;
    j["stones"]["team0"] = v.stones[0];
    j["stones"]["team1"] = v.stones[1];
    j["scores"]["team0"] = v.scores[0];
    j["scores"]["team1"] = v.scores[1];
    j["extra_end_score"]["team0"] = v.extra_end_score[0];
    j["extra_end_score"]["team1"] = v.extra_end_score[1];
    j["thinking_time_remaining"]["team0"] = v.thinking_time_remaining[0];
    j["thinking_time_remaining"]["team1"] = v.thinking_time_remaining[1];
    j["game_result"] = v.game_result;

}

void from_json(nlohmann::json const& j, GameState& v)
{
    j.at("end").get_to(v.end);
    j.at("shot").get_to(v.shot);
    j.at("hammer").get_to(v.hammer);
    j.at("stones").at("team0").get_to(v.stones[0]);
    j.at("stones").at("team1").get_to(v.stones[1]);
    j.at("scores").at("team0").get_to(v.scores[0]);
    j.at("scores").at("team1").get_to(v.scores[1]);
    j.at("extra_end_score").at("team0").get_to(v.extra_end_score[0]);
    j.at("extra_end_score").at("team1").get_to(v.extra_end_score[1]);
    j.at("thinking_time_remaining").at("team0").get_to(v.thinking_time_remaining[0]);
    j.at("thinking_time_remaining").at("team1").get_to(v.thinking_time_remaining[1]);
    j.at("game_result").get_to(v.game_result);
}


} // namespace digitalcurling3
