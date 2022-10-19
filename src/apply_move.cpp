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

#include "digitalcurling3/detail/apply_move.hpp"

#include <cassert>
#include <stdexcept>
#include <limits>
#include "digitalcurling3/detail/coordinate.hpp"
#include "digitalcurling3/detail/constants.hpp"

namespace digitalcurling3 {

namespace {



/// \brief ティーの座標
constexpr Vector2 kTee(0.f, coordinate::GetTeeLineY(true, coordinate::Id::kSimulation));


// シミュレータでシミュレートできるストーン数が，ショット数より多いことを確認
static_assert(GameState::kShotPerEnd <= ISimulator::kStoneMax);



/// \brief ショットの回転方向からストーンの角速度を算出する．
///
/// 現在，ストーンの角速度は ± π / 2 [rad/s] としている．
/// 
/// \param shot_rotation ショットの回転方向
/// \return ストーンの角速度 (rad/s)
inline float GetShotAngularVelocity(moves::Shot::Rotation shot_rotation)
{
    float angular_velocity_sign = 1.f;
    switch (shot_rotation) {
        case moves::Shot::Rotation::kCCW:
            angular_velocity_sign = 1.f;
            break;
        case moves::Shot::Rotation::kCW:
            angular_velocity_sign = -1.f;
            break;
        default:
            assert(false);
            break;
    }

    return kPi / 2.f * angular_velocity_sign;
}


/// \brief サイド毎に異なる座標をひとまとめにする．
///
/// シミュレーション座標系上ではシートのどちらのサイドからストーンを投げるかでハウスや判定ラインの位置が異なってくる．
/// そのため，この関数で座標変換を行い，判定用の共通の座標系に変換している．
/// そして，この判定用座標系上でスコア等の判定を行うことで処理を簡単にしている．
/// 具体的にこの関数は，ストーンがサイド1から投げられた場合に符号を反転し，サイド0側の判定と合わせている．
///
/// \param pos_sim ストーンの位置(シミュレーション座標系)
/// \param shot_side ショットを行ったサイド(coordinate::Id::kShot0 あるいは coordinate::Id::kShot1)
/// \return 判定用座標系上のストーン位置
inline Vector2 CanonicalizePositionOnSheet(Vector2 pos_sim, coordinate::Id shot_side)
{
    switch (shot_side) {
        case coordinate::Id::kShot0:
            return pos_sim;
        case coordinate::Id::kShot1:
            return -pos_sim;
        default:
            assert(false);
            return Vector2();  // 到達しない
    }
}


/// \brief シミュレーション中(ストーンが動いている間)にストーンが有効範囲内にいるかを判定する
///
/// \param stone_position ストーンの位置(シミュレーション座標系)
/// \param sheet_width シートの横幅
/// \param shot_side ショットを行ったサイド(coordinate::Id::kShot0 あるいは coordinate::Id::kShot1)
/// \return ストーンが有効範囲内にいる場合 \c true
inline bool IsStoneValidWhileSimulation(Vector2 stone_position, float sheet_width, coordinate::Id shot_side)
{
    Vector2 const canonical_stone_position = CanonicalizePositionOnSheet(stone_position, shot_side);
    return canonical_stone_position.x + ISimulator::kStoneRadius < sheet_width / 2.f
        && canonical_stone_position.x - ISimulator::kStoneRadius > -sheet_width / 2.f
        && canonical_stone_position.y - ISimulator::kStoneRadius < coordinate::GetBackLineY(true, coordinate::Id::kSimulation)  // バックラインの内側判定(例外的なので注意)
        && canonical_stone_position.y - ISimulator::kStoneRadius > -coordinate::GetBackBoardY(true, coordinate::Id::kSimulation);  // バックボードの内側

    // バックラインの内側判定 について
    // ストーンがバックラインに少しでも掛かっていれば内側と見なされるため
    //     stone_position.y - ISimulator::kStoneRadius < kBackLineYOnSheet
    // という判定にしている．
}


/// \brief ストーン停止時にストーンがプレイエリア内にいるかを判定する．
///
/// この関数はストーン停止時のストーン除外処理に使用する．
/// \warning ここではホグラインを超えているかの判定しかしない．
/// それ以外の判定は IsStoneValidWhileSimulation() ですでに行われているため．
///
/// \param stone_position ストーンの位置(シミュレーション座標系)
/// \param shot_side ショットを行ったサイド(coordinate::Id::kShot0 あるいは coordinate::Id::kShot1)
/// \return ストーンがホグラインを超えている場合 \c true
inline bool IsStoneInPlayArea(Vector2 stone_position, coordinate::Id shot_side)
{
    Vector2 const canonical_stone_position = CanonicalizePositionOnSheet(stone_position, shot_side);
    return canonical_stone_position.y - ISimulator::kStoneRadius > coordinate::GetHogLineY(true, coordinate::Id::kSimulation);
}

/// \brief ストーンがハウス内にいるかを判定する
/// \param stone_position ストーンの位置(シミュレーション座標系)
/// \param shot_side ショットを行ったサイド(coordinate::Id::kShot0 あるいは coordinate::Id::kShot1)
/// \return ストーンがハウス内にいる場合 \c true
inline bool IsStoneInHouse(Vector2 stone_position, coordinate::Id shot_side)
{
    Vector2 const canonical_stone_position = CanonicalizePositionOnSheet(stone_position, shot_side);
    return (canonical_stone_position - kTee).Length() < coordinate::kHouseRadius + ISimulator::kStoneRadius;
}


/// \brief ストーンがフリーガードゾーンにあるか否かを判定する．
///
/// 判定するのはハウス内かとティーラインの判定のみ．
/// それ以外の判定は IsStoneValidWhileSimulation() と IsStoneInPlayArea() で行う．
///
/// \param stone_position ストーンの位置(シミュレーション座標系)
/// \param shot_side ショット座標系
/// \return ストーンがフリーガードゾーン内にあれば \c true
inline bool IsStoneInFreeGuardZone(Vector2 stone_position, coordinate::Id shot_side)
{
    if (IsStoneInHouse(stone_position, shot_side)) {
        return false;
    }

    Vector2 const canonical_stone_position = CanonicalizePositionOnSheet(stone_position, shot_side);
    return canonical_stone_position.y + ISimulator::kStoneRadius < coordinate::GetTeeLineY(true, coordinate::Id::kSimulation);
}

/// \brief スコアを算出する
/// \param stones すべてのストーンの位置(シミュレーション座標系)
/// \param shot_side ショット座標系
/// \return first: チーム0のスコア， second: チーム1のスコア
inline std::pair<std::uint8_t, std::uint8_t> CheckScore(
    ISimulator::AllStones const& stones, coordinate::Id shot_side)
{
    // 全ストーンのティーからの位置を計算し，格納．
    std::array<float, GameState::kShotPerEnd> distances{};
    for (std::uint8_t i = 0; i < distances.size(); ++i) {
        if (stones[i]) {
            auto const canonical_stone_position = CanonicalizePositionOnSheet(stones[i]->position, shot_side);
            distances[i] = (canonical_stone_position - kTee).Length();
        } else {
            distances[i] = std::numeric_limits<float>::max();  // ストーンが存在しない場合は最大値
        }
    }

    // プレイヤー0のNo.1ストーンまでの距離をminDistance[0]に
    // プレイヤー1のNo.1ストーンまでの距離をminDistance[1]に格納．
    std::array<float, 2> min_distance{ {
        coordinate::kHouseRadius + ISimulator::kStoneRadius,
        coordinate::kHouseRadius + ISimulator::kStoneRadius} };

    for (std::uint8_t i = 0; i < distances.size(); ++i) {
        Team const team = GameState::StonesIndexFromAllStonesIndex(i).first;
        auto const team_idx = static_cast<size_t>(team);
        if (distances[i] < min_distance[team_idx]) {
            min_distance[team_idx] = distances[i];
        }
    }

    std::uint8_t team0_score = 0;
    std::uint8_t team1_score = 0;

    if (min_distance[0] < min_distance[1]) {
        // チーム0の得点
        for (size_t i = 0; i < GameState::kShotPerEnd / 2; ++i) { // チーム0のストーンを列挙
            if (distances[i] < min_distance[1]) {  // min_distance[*]は最大でもハウス半径+石半径になっているので，ハウス内の判定も同時に行える．
                ++team0_score;
            }
        }
    } else {
        // チーム1の得点
        for (size_t i = GameState::kShotPerEnd / 2; i < GameState::kShotPerEnd; ++i) {  // チーム1のストーンを列挙
            if (distances[i] < min_distance[0]) {
                ++team1_score;
            }
        }
    }

    return { team0_score, team1_score };
}


} // unnamed namespace


void ApplyMove(
    GameSetting const& setting,
    ISimulator & simulator,
    IPlayer & player,
    GameState & state,
    Move & move,
    std::chrono::milliseconds const& thinking_time_used,
    ApplyMoveResult * result,
    std::function<void(ISimulator const &)> on_step)
{
    // AllStonesのインデックス0~7はチーム0のストーン，インデックス8~16はチーム1のストーンになります．
    // 例えば，チーム1がハンマーの場合，各ストーンは AllStones の 0, 8, 1, 9, 2, 10, ... 番目に順番に格納されていきます．

    // ゲームが既に終了している
    if (state.IsGameOver()) {
        throw std::invalid_argument("game is already over");
    }

    if (state.shot >= GameState::kShotPerEnd) {
        throw std::invalid_argument("state.shot");
    }

    if (state.scores[0].size() < setting.max_end
        || state.scores[1].size() < setting.max_end) {
        throw std::out_of_range("state.scores");
    }

    if (state.hammer == Team::kInvalid) {
        throw std::invalid_argument("state.hammer");
    }

    for (auto const& stones : state.stones) {
        if (stones.size() != GameState::kShotPerEnd / 2) {
            throw std::invalid_argument("state.stones.size()");
        }
    }

    auto const shot_side = coordinate::GetShotSide(state.end);
    auto const current_team = state.GetNextTeam();
    auto const current_team_opponent = GetOpponentTeam(current_team);
    auto const current_team_stone_idx = state.shot / 2;

    // 制限時間の判定
    bool const is_time_limit_over = state.thinking_time_remaining[static_cast<size_t>(current_team)] <= thinking_time_used;

    // コンシードまたは時間切れの際 false
    bool const is_shot = std::holds_alternative<moves::Shot>(move) && !is_time_limit_over;

    if (is_shot) {

        // 乱数を加えた新規ショットの生成
        moves::Shot const new_shot = player.Play(std::get<moves::Shot>(move));

        // ショット前のストーンを保存しておく
        ISimulator::AllStones const pre_shot_stones = GameState::StonesToAllStones(state.stones, state.end);

        // シミュレーション: 初期状態を設定
        {
            ISimulator::AllStones initial_stones = pre_shot_stones;

            // 新しいストーンを生成．
            initial_stones[GameState::StonesIndexToAllStonesIndex(current_team, current_team_stone_idx)].emplace(
                coordinate::TransformPosition(Vector2(0.f, 0.f), shot_side, coordinate::Id::kSimulation),
                coordinate::TransformAngle(0.f, shot_side, coordinate::Id::kSimulation),
                coordinate::TransformVelocity(new_shot.velocity, shot_side, coordinate::Id::kSimulation),
                coordinate::TransformAngularVelocity(GetShotAngularVelocity(new_shot.rotation), shot_side, coordinate::Id::kSimulation));

            simulator.SetStones(initial_stones);
        }

        // シミュレーション: すべてのストーンが停止するまでループ
        while (true) {
            // シート外に出たストーンを除外
            ISimulator::AllStones stones = simulator.GetStones();
            bool stone_removed = false;
            for (auto & stone : stones) {
                if (stone && !IsStoneValidWhileSimulation(stone->position, setting.sheet_width, shot_side)) {
                    stone = std::nullopt;
                    stone_removed = true;
                }
            }
            if (stone_removed) {
                simulator.SetStones(stones);
            }

            // コールバック
            if (on_step) {
                on_step(simulator);
            }

            // ストーン停止判定
            if (simulator.AreAllStonesStopped()) {
                break;
            }

            // ステップ
            simulator.Step();
        }

        // すべてのストーンが停止した後，プレイエリア外のストーンを除外
        {
            ISimulator::AllStones stones = simulator.GetStones();
            bool stone_removed = false;
            for (auto & stone : stones) {
                if (stone && !IsStoneInPlayArea(stone->position, shot_side)) {
                    stone = std::nullopt;
                    stone_removed = true;
                }
            }
            if (stone_removed) {
                simulator.SetStones(stones);
            }
        }

        // フリーガードゾーンルールのチェック
        bool const free_guard_zone_foul = [&] {
            bool free_guard_zone_foul_tmp = false;
            std::uint8_t const free_guard_zone_count = setting.five_rock_rule ? 5 : 4;
            if (state.shot < free_guard_zone_count) {
                ISimulator::AllStones const& stones = simulator.GetStones();
                for (size_t team_stone_idx = 0; team_stone_idx < GameState::kShotPerEnd / 2; ++team_stone_idx) {
                    auto const opponent_stone_idx = GameState::StonesIndexToAllStonesIndex(current_team_opponent, team_stone_idx);
                    if (pre_shot_stones[opponent_stone_idx]
                        && IsStoneInFreeGuardZone(pre_shot_stones[opponent_stone_idx]->position, shot_side)
                        && !stones[opponent_stone_idx]) {
                        free_guard_zone_foul_tmp = true;
                        break;
                    }
                }
            }
            return free_guard_zone_foul_tmp;
        }();

        // フリーガードゾーンルールに抵触している場合，ショットの前の状態に戻す
        if (free_guard_zone_foul) {
            simulator.SetStones(pre_shot_stones);
        }

        // move に格納
        move = new_shot;

        // result に格納
        if (result) {
            result->free_guard_zone_foul = free_guard_zone_foul;
        }

    } else { // !is_shot
        // シミュレータに state.stones を反映
        simulator.SetStones(GameState::StonesToAllStones(state.stones, state.end));

        // result に格納
        if (result) {
            result->free_guard_zone_foul = false;
        }
    }

    // 思考時間更新 (Exエンド開始時の思考時間リセットはこの後で行われる)
    if (is_time_limit_over) {
        state.thinking_time_remaining[static_cast<size_t>(current_team)] = std::chrono::milliseconds(0);
    } else {
        state.thinking_time_remaining[static_cast<size_t>(current_team)] -= thinking_time_used;
    }

    // 以下 state の更新

    if (static_cast<size_t>(state.shot) < GameState::kShotPerEnd - 1) { // エンド中
        state.stones = GameState::StonesFromAllStones(simulator.GetStones(), state.end);
        ++state.shot;

    } else { // エンド終了時

        // スコア算出
        auto const [team0_score, team1_score] = CheckScore(simulator.GetStones(), shot_side);

        // スコアを反映
        if (state.end < setting.max_end) {  // 通常エンド
            state.scores[0][state.end] = team0_score;
            state.scores[1][state.end] = team1_score;
        } else if (team0_score != team1_score) {  // エクストラエンドの場合はスコア差がない場合は std::nullopt のまま
            state.extra_end_score[0] = team0_score;
            state.extra_end_score[1] = team1_score;
        }

        // 手番の変更．スコアが0(ブランクエンド)の時は変更されない．
        if (team0_score > 0) {
            state.hammer = Team::k1;
        } else if (team1_score > 0) {
            state.hammer = Team::k0;
        }

        // ストーンのリセット
        for (auto & team_stones : state.stones) {
            for (auto& stone : team_stones) {
                stone = std::nullopt;
            }
        }

        // エンドとショットを更新
        state.shot = 0;
        ++state.end;

        if (state.end >= setting.max_end) {  // 最後の非延長エンド終了以降
            // 勝敗の決定
            auto const team0_total_score = state.GetTotalScore(Team::k0);
            auto const team1_total_score = state.GetTotalScore(Team::k1);
            if (team0_total_score > team1_total_score) {
                state.hammer = Team::kInvalid;
                state.game_result.emplace();
                state.game_result->winner = Team::k0;
                state.game_result->reason = GameResult::Reason::kScore;
            } else if (team0_total_score < team1_total_score) {
                state.hammer = Team::kInvalid;
                state.game_result.emplace();
                state.game_result->winner = Team::k1;
                state.game_result->reason = GameResult::Reason::kScore;
            } else if (state.end >= GameState::kExtraEndMax) {  // スコア差無し かつ 延長エンド数限界
                state.hammer = Team::kInvalid;
                state.game_result.emplace();
                state.game_result->winner = Team::kInvalid;
                state.game_result->reason = GameResult::Reason::kDraw;
            } else { // 勝敗が決定しなかった場合，延長エンドとして続行
                // 延長エンドの残り思考時間を適用
                for (size_t i = 0; i < 2; ++i) {
                    state.thinking_time_remaining[i] = setting.extra_end_thinking_time[i];
                }
            }
        }
    }

    // コンシードや時間切れの場合はstateを上書きする．
    if (!is_shot) {
        state.hammer = Team::kInvalid;

        state.game_result.emplace();
        switch (current_team) {  // コンシードや時間切れの場合，行動を行ったチームが負け
            case Team::k0:
                state.game_result->winner = Team::k1;
                break;
            case Team::k1:
                state.game_result->winner = Team::k0;
                break;
            default:
                assert(false);
                break;
        }

        if (is_time_limit_over) {  // 時間切れかつコンシードの場合は時間切れになる
            state.game_result->reason = GameResult::Reason::kTimeLimit;
        } else {
            state.game_result->reason = GameResult::Reason::kConcede;
        }
    }
}


} // namespace digitalcurling3
