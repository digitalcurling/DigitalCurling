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
/// \brief GameState を定義します

#ifndef DIGITALCURLING3_GAME_STATE_HPP
#define DIGITALCURLING3_GAME_STATE_HPP

#include <cstdint>
#include <array>
#include <optional>
#include <vector>
#include <chrono>
#include "json/common.hpp"
#include "vector2.hpp"
#include "transform.hpp"
#include "constants.hpp"
#include "game_result.hpp"
#include "game_setting.hpp"
#include "i_simulator.hpp"

namespace digitalcurling3 {




/// \brief 試合の状態を表す
///
/// \note この構造体はカプセル化していません．
/// これは構造体内部の値を直接書き換えることで様々な盤面の状態を作れるようにするためです．
/// 構造体内部の値を直接書き換えることによって，内部の値が不正になる可能性がありますが，
/// そのような内部の値が不正な GameState インスタンスを使用した場合の動作は保証しません．
struct GameState {

    /// \brief 延長戦を含めたエンド数の最大値 ( GameState::end >= kExtraEndMax のときは無効なエンドを表します)
    static constexpr std::uint8_t kExtraEndMax = 255;

    /// \brief エンド毎のショット数
    static constexpr size_t kShotPerEnd = 16;

    /// \brief 各チームのストーンの位置と角度を格納します
    /// \sa GameState::stones
    using Stones = std::array<
        std::array<std::optional<Transform>, kShotPerEnd / 2>,
        2>;

    /// \brief デフォルトコンストラクタ．
    ///
    /// \note このコンストラクタは便宜上用意されたものです．
    /// このコンストラクタで生成されたインスタンスをそのまま ApplyMove() に渡さないようにしてください．
    /// 試合の初期盤面を構築する場合は代わりに GameState(GameSetting const&) を用いてください．
    ///
    /// \sa GameState(GameSetting const&) 試合設定に対する初期盤面を構築する
    GameState();

    /// \brief 引数で指定された試合設定に対する初期盤面を構築する
    ///
    /// \param[in] setting 試合設定
    GameState(GameSetting const& setting);

    /// \brief コピーする
    GameState(GameState const&) = default;

    /// \brief ムーブする
    ///
    /// \note ムーブ済みオブジェクトに対するあらゆる操作は保証されません．
    GameState(GameState &&) = default;

    /// \brief コピーする
    GameState& operator = (GameState const&) = default;

    /// \brief ムーブする
    ///
    /// \note ムーブ済みオブジェクトに対するあらゆる操作は保証されません．
    GameState& operator = (GameState &&) = default;

    /// \brief 現在のエンド．
    ///
    /// \ref kExtraEndMax になると強制的に引き分けとして処理されます．
    std::uint8_t end;

    /// \brief 現在のショット番号．0以上，15以下．
    std::uint8_t shot;

    /// \brief 現在のエンドのハンマー(後攻)．
    ///
    /// ゲーム終了後は Team::kInvalid になります．
    Team hammer;

    /// \brief 各チームのストーンの位置と角度
    ///
    /// 1つ目のインデックスはチーム，2つ目のインデックスはストーンのインデックスを表します．
    /// \code {.cpp}
    /// stones[0][0]  // チーム0の0番目のストーン
    /// stones[1][7] // チーム1の7番目のストーン
    /// \endcode
    ///
    /// ストーンが盤面に存在しない場合は \c std::nullopt になります．
    ///
    /// 座標系はストーンの発射地点(ハックの位置)を原点，ショットする方向(反対側のハウスがある向き)をy軸正方向としています．
    Stones stones;

    /// \brief 各エンドのスコアを格納する
    ///
    /// 1つ目のインデックスはチーム，2つ目のインデックスはエンドを表します．
    /// \code {.cpp}
    /// scores[0][0]  // チーム0の0エンド目のスコア
    /// scores[1][9]  // チーム1の9エンド目のスコア
    /// \endcode
    ///
    /// スコアが未確定の場合は \c std::nullopt になります．
    ///
    /// \sa extra_end_score 延長エンドのスコア
    std::array<
        std::vector<std::optional<std::uint8_t>>,
        2> scores;

    /// \brief エクストラエンド(延長戦)のスコア．
    ///
    /// インデックスはチームを表します．
    /// \code {.cpp}
    /// extra_end_score[0]  // チーム0のエクストラエンドのスコア
    /// extra_end_score[1]  // チーム1のエクストラエンドのスコア
    /// \endcode
    ///
    /// スコアが未確定の場合は \c std::nullopt になります．
    /// エクストラエンドの得点が0以外になったとき(つまり延長戦によって勝敗が決定したとき)のみ，
    /// 有効な(\c std::nullopt 以外の)値が格納されます．
    std::array<std::optional<std::uint8_t>, 2> extra_end_score;

    /// \brief 各チームの残り思考時間
    ///
    /// 0になると時間切れ負け
    std::array<std::chrono::milliseconds, 2> thinking_time_remaining;

    /// \brief 試合結果
    ///
    /// \c std::nullopt の値は試合中を意味します
    std::optional<GameResult> game_result;

    /// \brief チームの現在までの合計スコアを得る
    ///
    /// \param[in] team チーム． Team::kInvalid は渡さないでください．
    /// \returns 引数で指定したプレイヤーの合計スコア
    /// \exception std::invalid_argument 引数が不正な場合
    std::uint32_t GetTotalScore(Team team) const;

    /// \brief 次に行動するチームを得る．
    ///
    /// ゲームがすでに終了している場合 Team::kInvalid を返す．
    /// \returns 次に行動するチーム
    /// \exception std::logic_error 構造体の内容が不正な場合
    Team GetNextTeam() const;

    /// \brief ゲームが終了しているかを調べる
    ///
    /// \return ゲームが終了している場合 \c true
    bool IsGameOver() const
    {
        return game_result.has_value();
    }

    /// \brief ISimulator::AllStones のインデックスから GameState::Stones のインデックスに変換する．
    ///
    /// \param all_stones_index ISimulator::AllStones のインデックス
    /// \return first: チーム, second: チーム内のストーンのインデックス
    static std::pair<Team, size_t> StonesIndexFromAllStonesIndex(size_t all_stones_index);

    /// \brief GameState::Stones のインデックスから ISimulator::AllStones のインデックスに変換する．
    ///
    /// \param team チーム
    /// \param team_stone_index チーム内のストーンのインデックス
    /// \return ISimulator::AllStones のインデックス
    static size_t StonesIndexToAllStonesIndex(Team team, size_t team_stone_index);

    /// \brief ISimulator::AllStones から GameState::Stones へ変換する．
    ///
    /// \param all_stones ストーン
    /// \param end 現在のエンド
    /// \return \p all_stones を GameState::Stones に変換したもの 
    static Stones StonesFromAllStones(ISimulator::AllStones const& all_stones, std::uint8_t end);

    /// \brief GameState::Stones から ISimulator::AllStones へ変換する．
    ///
    /// \param stones ストーン
    /// \param end 現在のエンド
    /// \return \ref stones を ISimulator::AllStones に変換したもの
    static ISimulator::AllStones StonesToAllStones(Stones const& stones, std::uint8_t end);
};


/// \cond Doxygen_Suppress
// json
void to_json(nlohmann::json &, GameState const&);
void from_json(nlohmann::json const&, GameState &);
/// \endcond

} // namespace digitalcurling3

#endif // DIGITALCURLING3_GAME_STATE_HPP
