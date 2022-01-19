#ifndef DIGITAL_CURLING_GAME_STATE_HPP
#define DIGITAL_CURLING_GAME_STATE_HPP

#include <cstdint>
#include <array>
#include <optional>
#include "../json.hpp"
#include "../vector2.hpp"
#include "../constants.hpp"
#include "constants.hpp"
#include "result.hpp"

namespace digital_curling::game {

/// <summary>
/// 試合の状態を表す．
/// </summary>
struct State {
    /// <summary>
    /// 初期盤面を構築する．
    /// </summary>
    State();

    State(State const&) = default;
    State& operator = (State const&) = default;

    /// <summary>
    /// ストーンの座標．
    /// 座標系はストーンの発射地点を(0, 0)，ショットする方向(反対側のハウスがある向き)をy軸正方向とする．
    /// </summary>
    std::array<std::optional<Vector2>, kStoneMax> stone_positions;

    /// <summary>
    /// 0エンド目から<see cref="current_end"/>エンド目(最大で<see cref="Setting::end"/>)までのスコアを格納する．
    /// 各スコアは正ならチーム0，負ならチーム1の得点．0はブランクエンドを意味する．
    /// </summary>
    /// <remarks>
    /// 延長エンドのスコアは別に<see cref="extra_end_score"/>に格納される．
    /// </remarks>
    std::array<std::int8_t, kEndMax> scores;

    /// <summary>
    /// 現在のショット番号．<c>0 &lt;= current_shot &lt;= 15</c>
    /// </summary>
    std::uint8_t current_shot;

    /// <summary>
    /// 現在のエンドの先行．ゲーム終了後は<see cref="Team::kInvalid"/>が格納される．
    /// </summary>
    Team current_end_first;

    /// <summary>
    /// 現在のエンド．<c>0 &lt;= current_end</c>
    /// </summary>
    std::uint8_t current_end;

    /// <summary>
    /// 延長エンドのスコア．
    /// </summary>
    std::int8_t extra_end_score;

    /// <summary>
    /// 試合結果．<c>std::nullopt</c>の間は試合中．
    /// </summary>
    std::optional<Result> result;

    /// <summary>
    /// チームの現在までの合計スコアを得る
    /// </summary>
    /// <param name="team">チームID．<see cref="Team::kInvalid"/>は渡さないでください．</param>
    /// <returns>引数で指定したプレイヤーの合計スコア</returns>
    std::uint32_t GetScore(Team team) const;

    /// <summary>
    /// 現在ショットを行うチームを得る．ゲームがすでに終了している場合<see cref="Team::kInvalid"/>を返す．
    /// </summary>
    /// <returns>現在ショットを行うプレイヤー</returns>
    Team GetCurrentTeam() const;
};

} // namespace digital_curling::game



// json

namespace digital_curling::game {

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    State,
    stone_positions,
    scores,
    current_shot,
    current_end_first,
    current_end,
    extra_end_score,
    result
)

} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_STATE_HPP
