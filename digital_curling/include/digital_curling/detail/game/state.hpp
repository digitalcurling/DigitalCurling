/// \file
/// \brief State を定義します

#ifndef DIGITAL_CURLING_GAME_STATE_HPP
#define DIGITAL_CURLING_GAME_STATE_HPP

#include <cstdint>
#include <array>
#include <optional>
#include "../vector2.hpp"
#include "../constants.hpp"
#include "constants.hpp"
#include "result.hpp"

namespace digital_curling::game {

/// \brief 試合の状態を表す
struct State {
    /// \brief 初期盤面を構築する
    State();

    /// \brief 盤面をコピーする
    State(State const&) = default;

    /// \brief 盤面をコピーする
    State& operator = (State const&) = default;

    /// \brief ストーンの座標．
    ///
    /// 座標系はストーンの発射地点(ハックの位置)を原点，ショットする方向(反対側のハウスがある向き)をy軸正方向としています．
    std::array<std::optional<Vector2>, kStoneMax> stone_positions;

    /// \brief 0エンド目から \ref current_end エンド目(最大で Setting::end )までのスコアを格納する．
    ///
    /// 各スコアは正ならチーム0，負ならチーム1の得点．0はブランクエンドを意味する．
    ///
    /// 延長エンドのスコアは別に \ref extra_end_score に格納される．
    std::array<std::int8_t, kEndMax> scores;

    /// \brief 現在のショット番号．0以上，15以下．
    std::uint8_t current_shot;

    /// \brief 現在のエンドの先行．
    ///
    /// ゲーム終了後は Team::kInvalid が格納される．
    Team current_end_first;

    /// \brief 現在のエンド．0以上．
    std::uint8_t current_end;

    /// \brief 延長エンドのスコア．
    std::int8_t extra_end_score;

    /// \brief 試合結果
    /// \c std::nullopt の値は試合中を意味します
    std::optional<Result> result;

    /// \brief チームの現在までの合計スコアを得る
    ///
    /// \param[in] team チームID． Team::kInvalid は渡さないでください．
    /// \returns 引数で指定したプレイヤーの合計スコア
    std::uint32_t GetScore(Team team) const;

    /// \brief 現在ショットを行うチームを得る．
    ///
    /// ゲームがすでに終了している場合 Team::kInvalid を返す．
    /// \returns 現在ショットを行うプレイヤー
    Team GetCurrentTeam() const;
};

} // namespace digital_curling::game

#endif // DIGITAL_CURLING_GAME_STATE_HPP
