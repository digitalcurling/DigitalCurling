// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief GameState を定義

#pragma once

#include <cstdint>
#include <optional>
#include <chrono>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "digitalcurling/common.hpp"
#include "digitalcurling/game_scores.hpp"
#include "digitalcurling/game_setting.hpp"
#include "digitalcurling/game_result.hpp"
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
        , scores()
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
        , scores(setting.max_end)
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
    /// スコアが未確定・エクストラエンドなし などの場合は `std::nullopt` になります。
    ///
    /// Ex.) 10エンドゲームの場合の例
    /// ```cpp
    /// scores[Team::k0][0]  // チーム0の1エンド目のスコア
    /// scores[Team::k0][9]  // チーム0の10エンド目のスコア
    /// scores[Team::k1][10] // チーム1のエクストラエンドのスコア
    /// ```
    GameScores scores;

    /// @brief 各チームの残り思考時間
    ///
    /// 0になると時間切れにより負けとなります。
    TeamValue<std::chrono::milliseconds> thinking_time_remaining;

    /// @brief 試合結果
    ///
    /// `std::nullopt` の値は試合中を意味します。
    std::optional<GameResult> game_result;

    /// @brief 次に行動するチームを得る
    /// @returns 次に行動するチーム (ゲームがすでに終了している場合 `Team::kInvalid`)
    /// @exception std::logic_error 構造体の内容が不正な場合
    Team GetNextTeam() const {
        if (IsGameOver()) return Team::kInvalid;

        auto res = GetOpponentTeam(hammer);
        return res != Team::kInvalid ? res : throw std::logic_error("invalid state");
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
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameState, end, shot, hammer, stones, scores, thinking_time_remaining, game_result)
/// @endcond

} // namespace digitalcurling
