// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief GameState を定義

#pragma once

#include <cstdint>
#include <array>
#include <optional>
#include <vector>
#include <chrono>
#include "digitalcurling/common.hpp"
#include "digitalcurling/game_result.hpp"
#include "digitalcurling/game_setting.hpp"
#include "digitalcurling/stone.hpp"
#include "digitalcurling/stone_coordinate.hpp"
#include "digitalcurling/team.hpp"
#include "digitalcurling/vector2.hpp"

namespace digitalcurling {


/// @brief 試合の状態
///
/// @note この構造体はカプセル化していません。
/// これは構造体内部の値を直接書き換えることで様々な盤面の状態を作れるようにするためです。
/// 構造体内部の値を直接書き換えることによって、内部の値が不正になる可能性がありますが、
/// そのような内部の値が不正な GameState インスタンスを使用した場合の動作は保証しません。
struct GameState {

    /// @brief 延長戦を含めたエンド数の最大値 ( GameState::end >= kExtraEndMax のときは無効なエンドを表します)
    static constexpr std::uint8_t kExtraEndMax = 255;

    /// @brief デフォルトコンストラクタ
    /// @note このコンストラクタは便宜上用意されたものです。
    /// 試合の初期盤面を構築する場合は代わりに `GameState(GameSetting const&)` を使用してください。
    /// @sa GameState(GameSetting const&) 試合設定に対する初期盤面を構築する
    GameState()
        : end(0)
        , shot(0)
        , hammer(Team::k1)
        , stones()
        , scores{{ {}, {} }}
        , extra_end_score{{}}
        , thinking_time_remaining()
        , game_result()
    {}

    /// @brief 引数で指定された試合設定に対する初期盤面を構築する
    /// @param[in] setting 試合設定
    GameState(GameSetting const& setting)
        : end(0)
        , shot(0)
        , hammer(Team::k1)
        , stones()
        , scores{{ std::vector<std::optional<std::uint8_t>>(setting.max_end),
                std::vector<std::optional<std::uint8_t>>(setting.max_end) }}
        , extra_end_score{{}}
        , thinking_time_remaining(setting.thinking_time)
        , game_result()
    {}

    /// @brief コピーする
    GameState(GameState const&) = default;

    /// @brief ムーブする
    /// @note ムーブ済みオブジェクトに対するあらゆる操作は保証されません
    GameState(GameState &&) = default;

    /// @brief コピーする
    GameState& operator = (GameState const&) = default;

    /// @brief ムーブする
    /// @note ムーブ済みオブジェクトに対するあらゆる操作は保証されません
    GameState& operator = (GameState &&) = default;

    /// @brief 現在のエンド
    /// @note `kExtraEndMax` になると強制的に引き分けとして処理されます。
    /// @sa kExtraEndMax
    std::uint8_t end;

    /// @brief 現在のショット番号 (0～15)
    std::uint8_t shot;

    /// @brief 現在のエンドのハンマー(後攻)
    ///
    /// ゲーム終了後は `Team::kInvalid` になります。
    Team hammer;

    /// @brief 各チームのストーンの位置と角度
    ///
    /// ストーンが盤面に存在しない場合は `std::nullopt` になります。
    ///
    /// Ex.)
    /// ```cpp
    /// stones[Team::k0][0] // チーム0の1番目のストーン
    /// stones[Team::k1][7] // チーム1の8番目のストーン
    /// ```
    StoneCoordinate stones;

    /// @brief 各エンドのスコアを格納
    ///
    /// 1つ目のインデックスはチーム、2つ目のインデックスはエンドを表します。
    /// ```cpp
    /// scores[0][0]  // チーム0の0エンド目のスコア
    /// scores[1][9]  // チーム1の9エンド目のスコア
    /// ```
    ///
    /// スコアが未確定の場合は `std::nullopt` になります。
    ///
    /// @sa extra_end_score 延長エンドのスコア
    std::array<
        std::vector<std::optional<std::uint8_t>>,
        2> scores;

    /// @brief エクストラエンド(延長戦)のスコア
    ///
    /// インデックスはチームを表します。
    /// ```cpp
    /// extra_end_score[0]  // チーム0のエクストラエンドのスコア
    /// extra_end_score[1]  // チーム1のエクストラエンドのスコア
    /// ```
    ///
    /// スコアが未確定の場合は `std::nullopt` になります。
    /// エクストラエンドの得点が0以外になったとき(つまり延長戦によって勝敗が決定したとき)のみ、
    /// 有効な(`std::nullopt` 以外の)値が格納されます。
    std::array<std::optional<std::uint8_t>, 2> extra_end_score;

    /// @brief 各チームの残り思考時間
    ///
    /// 0になると時間切れにより負けとなります。
    TeamValue<std::chrono::milliseconds> thinking_time_remaining;

    /// @brief 試合結果
    ///
    /// `std::nullopt` の値は試合中を意味します。
    std::optional<GameResult> game_result;

    /// @brief チームの現在までの合計スコアを得る
    /// @param[in] team チーム
    /// @returns 引数で指定したプレイヤーの合計スコア
    /// @exception std::invalid_argument 引数が不正な場合
    std::uint32_t GetTotalScore(Team team) const {
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

    /// @brief 次に行動するチームを得る
    /// @returns 次に行動するチーム (ゲームがすでに終了している場合 `Team::kInvalid`)
    /// @exception std::logic_error 構造体の内容が不正な場合
    Team GetNextTeam() const {
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

    /// @brief ゲームが終了しているかを調べる
    /// @return ゲームが終了している場合 `true`
    bool IsGameOver() const
    {
        return game_result.has_value();
    }
};


/// @cond Doxygen_Suppress
// json
inline void to_json(nlohmann::json& j, GameState const& v) {
    j["end"] = v.end;
    j["shot"] = v.shot;
    j["hammer"] = v.hammer;
    j["stones"] = v.stones;
    j["scores"]["team0"] = v.scores[0];
    j["scores"]["team1"] = v.scores[1];
    j["extra_end_score"]["team0"] = v.extra_end_score[0];
    j["extra_end_score"]["team1"] = v.extra_end_score[1];
    j["thinking_time_remaining"] = v.thinking_time_remaining;
    j["game_result"] = v.game_result;
}

inline void from_json(nlohmann::json const& j, GameState& v) {
    j.at("end").get_to(v.end);
    j.at("shot").get_to(v.shot);
    j.at("hammer").get_to(v.hammer);
    j.at("stones").get_to(v.stones);
    j.at("scores").at("team0").get_to(v.scores[0]);
    j.at("scores").at("team1").get_to(v.scores[1]);
    j.at("extra_end_score").at("team0").get_to(v.extra_end_score[0]);
    j.at("extra_end_score").at("team1").get_to(v.extra_end_score[1]);
    j.at("thinking_time_remaining").get_to(v.thinking_time_remaining);
    j.at("game_result").get_to(v.game_result);
}
/// @endcond

} // namespace digitalcurling
