#include "digital_curling/detail/game/normal.hpp"

#include <cassert>
#include <stdexcept>
#include <random>
#include <limits>
#include "digital_curling/detail/coordinate.hpp"

namespace digital_curling::game::normal {

namespace {

// overloadedトリック用ヘルパークラス
// 参考: https://dev.to/tmr232/that-overloaded-trick-overloading-lambdas-in-c17
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...)->Overloaded<Ts...>;

} // unnamed namespace



void to_json(nlohmann::json & j, Setting const& v)
{
    j["randomize_initial_shot_velocity"] = v.randomize_initial_shot_velocity;
    j["end"] = v.end;
    j["sheet_width"] = v.sheet_width;
    j["five_rock_rule"] = v.five_rock_rule;
    j["max_shot_speed"] = v.max_shot_speed;
    j["shot_randomizer"] = *v.shot_randomizer;
}

void from_json(nlohmann::json const& j, Setting & v)
{
    j.at("randomize_initial_shot_velocity").get_to(v.randomize_initial_shot_velocity);
    j.at("end").get_to(v.end);
    j.at("sheet_width").get_to(v.sheet_width);
    j.at("five_rock_rule").get_to(v.five_rock_rule);
    j.at("max_shot_speed").get_to(v.max_shot_speed);
    v.shot_randomizer = j.at("shot_randomizer").get<std::unique_ptr<IShotRandomizer>>();
}



State::State()
    : stone_positions()
    , scores({})
    , current_shot(0)
    , current_end_first(Team::k0)
    , current_end(0)
    , extra_end_score(0)
    , result()
{}

std::uint32_t State::GetScore(Team team) const
{
    assert(team != Team::kInvalid);

    std::uint32_t score_sum = 0;

    for (auto const score : scores) {
        if (team == Team::k0 && score > 0) {
            score_sum += score;
        } else if (team == Team::k1 && score < 0) {
            score_sum -= score;
        }
    }

    if (team == Team::k0 && extra_end_score > 0) {
        score_sum += extra_end_score;
    } else if (team == Team::k1 && extra_end_score < 0) {
        score_sum -= extra_end_score;
    }

    return score_sum;
}

Team State::GetCurrentTeam() const
{
    if (result) {  // ゲームがすでに終わっている．
        return Team::kInvalid;
    }

    switch (current_end_first) {
        case Team::k0:
            if (current_shot % 2 == 0) {
                return Team::k0;
            } else {
                return Team::k1;
            }
            break;  // 到達しない

        case Team::k1:
            if (current_shot % 2 == 0) {
                return Team::k1;
            } else {
                return Team::k0;
            }
            break;  // 到達しない

        case Team::kInvalid:
            assert(false);
            break;
    }

    return Team::k0;  // 到達しない
}



namespace {

constexpr Vector2 kTee(0.f, coordinate::GetTeeLineY(true, coordinate::Id::kSimulation));


inline float GetShotAngularVelocity(Shot::Rotation shot_rotation)
{
    float angular_velocity_sign = 1.f;
    switch (shot_rotation) {
        case Shot::Rotation::kCCW:
            angular_velocity_sign = 1.f;
            break;
        case Shot::Rotation::kCW:
            angular_velocity_sign = -1.f;
            break;
        default:
            assert(false);
            break;
    }

    return kPi / 2.f * angular_velocity_sign;
}


/// <summary>
/// 両サイドで異なるシート座標をサイド0側にひとまとめにする．
/// </summary>
/// <param name="pos_sim">シート座標系</param>
/// <param name="shot_side">サイド</param>
/// <returns>サイド0側のシート座標</returns>
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


/// <summary>
/// シミュレーション中(ストーンが動いている間)にストーンが有効範囲内にいるかを判定する
/// </summary>
/// <param name="stone_position"></param>
/// <param name="sheet_width"></param>
/// <param name="stone_radius"></param>
/// <param name="shot_side"></param>
/// <returns></returns>
inline bool IsStoneValidWhileSimulation(Vector2 stone_position, float sheet_width, float stone_radius, coordinate::Id shot_side)
{
    Vector2 const canonical_stone_position = CanonicalizePositionOnSheet(stone_position, shot_side);
    return canonical_stone_position.x + stone_radius < sheet_width / 2.f
        && canonical_stone_position.x - stone_radius > -sheet_width / 2.f
        && canonical_stone_position.y - stone_radius < coordinate::GetBackLineY(true, coordinate::Id::kSimulation)  // バックラインの内側判定(例外的なので注意)
        && canonical_stone_position.y - stone_radius > -coordinate::GetBackBoardY(true, coordinate::Id::kSimulation);  // バックボードの内側

    // バックラインの内側判定 について
    // ストーンがバックラインに少しでも掛かっていれば内側と見なされるため
    //     stone_position.y - stone_radius < kBackLineYOnSheet
    // という判定にしている．
}


/// <summary>
/// ストーン停止時のストーン除外判定に使う．
/// なお，ここではホグラインを超えているかの判定しかしない．
/// それ以外の判定は<see cref="IsStoneValidWhileSimulation"/>ですでに行われているため．
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="stone_position"></param>
/// <param name="stone_radius"></param>
/// <param name="shot_side"></param>
/// <returns></returns>
inline bool IsStoneInPlayArea(Vector2 stone_position, float stone_radius, coordinate::Id shot_side)
{
    Vector2 const canonical_stone_position = CanonicalizePositionOnSheet(stone_position, shot_side);
    return canonical_stone_position.y - stone_radius > coordinate::GetHogLineY(true, coordinate::Id::kSimulation);
}


inline bool IsStoneInHouse(Vector2 stone_position, float stone_radius, coordinate::Id shot_side)
{
    Vector2 const canonical_stone_position = CanonicalizePositionOnSheet(stone_position, shot_side);
    return (canonical_stone_position - kTee).Length() < coordinate::kHouseRadius + stone_radius;
}


/// <summary>
/// ストーンがフリーガードゾーンにあるか否かを判定する．
/// 判定するのはハウス内かとティーラインの判定のみ．
/// それ以外の判定は<see cref="IsStoneValidWhileSimulation"/>と<see cref="IsStoneInPlayArea"/>で行う．
/// </summary>
/// <param name="stone_position"></param>
/// <param name="stone_radius"></param>
/// <param name="sheet_side"></param>
/// <returns></returns>
inline bool IsStoneInFreeGuardZone(Vector2 stone_position, float stone_radius, coordinate::Id shot_side)
{
    if (IsStoneInHouse(stone_position, stone_radius, shot_side)) {
        return false;
    }

    Vector2 const canonical_stone_position = CanonicalizePositionOnSheet(stone_position, shot_side);
    return canonical_stone_position.y + stone_radius < coordinate::GetTeeLineY(true, coordinate::Id::kSimulation);
}


inline std::int8_t CheckScore(simulation::AllStoneData const& stones, float stone_radius, coordinate::Id shot_side, Team current_end_first)
{
    assert(current_end_first != Team::kInvalid);

    // 全ストーンのティーからの位置を計算し，格納．
    std::array<float, kStoneMax> distances{};
    for (StoneId i = 0; i < kStoneMax; ++i) {
        if (stones[i]) {
            auto const canonical_stone_position = CanonicalizePositionOnSheet(stones[i]->position, shot_side);
            distances[i] = (canonical_stone_position - kTee).Length();
        } else {
            distances[i] = std::numeric_limits<float>::max();
        }
    }

    // プレイヤー0のNo.1ストーンまでの距離をminDistance0に
    // プレイヤー1のNo.1ストーンまでの距離をminDistance1に格納．
    std::array<float, 2> minDistance{ {
        coordinate::kHouseRadius + stone_radius,
        coordinate::kHouseRadius + stone_radius} };

    for (StoneId i = 0; i < kStoneMax; ++i) {
        size_t const team_id = (i + static_cast<size_t>(current_end_first)) % 2;
        if (distances[i] < minDistance[team_id]) {
            minDistance[team_id] = distances[i];
        }
    }

    std::int8_t score = 0;

    if (minDistance[0] < minDistance[1]) {
        // プレイヤー0の得点
        for (StoneId i = static_cast<size_t>(current_end_first); i < kStoneMax; i += 2) {  // プレイヤー0のストーンを列挙
            if (distances[i] < minDistance[0]) {  // minDistance*は最大でもハウス半径+石半径になっているので，ハウス内の判定も同時に行える．
                score++;
            }
        }
    } else {
        // プレイヤー1の得点
        for (StoneId i = (static_cast<size_t>(current_end_first) + 1) % 2; i < kStoneMax; i += 2) {  // プレイヤー1のストーンを列挙
            if (distances[i] < minDistance[1]) {
                score--;
            }
        }
    }

    return score;
}


Vector2 RandomizeShotVelocity(Vector2 shot_velocity, float stddev_shot_speed, float stddev_shot_angle)
{
    thread_local auto random_engine = std::default_random_engine(std::random_device()());
    auto speed_random_dist = std::normal_distribution<float>(0.f, stddev_shot_speed);
    auto angle_random_dist = std::normal_distribution<float>(0.f, stddev_shot_angle);
    float const speed = shot_velocity.Length() + speed_random_dist(random_engine);
    float const angle = std::atan2(shot_velocity.y, shot_velocity.x) + angle_random_dist(random_engine);
    return speed * Vector2(std::cos(angle), std::sin(angle));
}



} // unnamed namespace


void ApplyMove(
    Setting const& setting,
    State & state,
    simulation::ISimulator & simulator,
    Move & move)
{
    using namespace coordinate;

    // TODO 例外時の保証

    // ゲームが既に終了している
    if (state.result) return;

    assert(state.current_end_first != Team::kInvalid);

    auto const shot_side = GetShotSide(state.current_end);
    float const stone_radius = simulator.GetStoneRadius();
    bool is_shot = std::holds_alternative<Shot>(move);

    // シート上のストーンの初期状態を設定

    simulation::AllStoneData initial_stones;
    {
        auto const& stones = simulator.GetStones();
        for (StoneId i = 0; i < kStoneMax; ++i) {
            if (i < state.current_shot && state.stone_positions[i]) {
                auto const stone_pos_sim = TransformPosition(*state.stone_positions[i], shot_side, Id::kSimulation);

                // ストーンの角度はシミュレータから取得する．
                // ただし，シミュレータ内でストーンを動かしていないと思われる場合のみ使用する．
                float stone_angle_sim = 0.f;
                constexpr float kEpsilon = 0.0001f;  // 許容誤差．この値は座標変換の誤差を考えて大きめにしている(FLT_EPSILONでは小さすぎる)が，ぶっちゃけ適当である．
                if (stones[i]
                    && std::abs(stones[i]->position.x - stone_pos_sim.x) <= kEpsilon
                    && std::abs(stones[i]->position.y - stone_pos_sim.y) <= kEpsilon) {
                    stone_angle_sim = stones[i]->angle;
                }

                initial_stones[i] = simulation::StoneData(
                    stone_pos_sim,
                    stone_angle_sim,
                    Vector2(0.f, 0.f),
                    0.f);
            } else {
                initial_stones[i] = std::nullopt;
            }
        }
    }

    if (is_shot) {
        Shot & shot = std::get<Shot>(move);
        // 最大速度制限を適用．
        if (auto speed = shot.velocity.Length(); speed > setting.max_shot_speed) {
            shot.velocity *= setting.max_shot_speed / speed;
        }
        // 乱数を加える
        if (setting.randomize_initial_shot_velocity) {
            shot.velocity = setting.shot_randomizer->Randomize(shot.velocity);
        }
        initial_stones[state.current_shot] = simulation::StoneData(
            TransformPosition(Vector2(0.f, 0.f), shot_side, Id::kSimulation),
            TransformAngle(0.f, shot_side, Id::kSimulation),
            TransformVelocity(shot.velocity, shot_side, Id::kSimulation),
            TransformAngularVelocity(GetShotAngularVelocity(shot.rotation), shot_side, Id::kSimulation));
    }

    simulator.SetStones(initial_stones);

    // シミュレーション
    if (is_shot) {
        // すべてのストーンが停止するまでループ
        while (!simulator.AreAllStonesStopped()) {
            simulator.Step();
            simulation::AllStoneData stones = simulator.GetStones();

            // シート外に出たストーンを除外
            bool stone_removed = false;
            for (StoneId i = 0; i <= state.current_shot; ++i) {
                if (stones[i] && !IsStoneValidWhileSimulation(stones[i]->position, setting.sheet_width, stone_radius, shot_side)) {
                    stones[i] = std::nullopt;
                    stone_removed = true;
                }
            }
            if (stone_removed) {
                simulator.SetStones(stones);
            }

            // コールバック
            if (setting.on_step) {
                setting.on_step(simulator);
            }
        }

        // すべてのストーンが停止した後，プレイエリア外のストーンを削除
        {
            simulation::AllStoneData stones = simulator.GetStones();
            bool stone_removed = false;
            for (StoneId i = 0; i <= state.current_shot; ++i) {
                if (stones[i] && !IsStoneInPlayArea(stones[i]->position, stone_radius, shot_side)) {
                    stones[i] = std::nullopt;
                    stone_removed = true;
                }
            }
            if (stone_removed) {
                simulator.SetStones(stones);
            }
        }

        // フリーガードゾーンルールの適用
        bool free_guard_faul = false;
        std::uint8_t const free_guard_zone_count = setting.five_rock_rule ? 5 : 4;
        if (state.current_shot < free_guard_zone_count) {
            simulation::AllStoneData const& stones = simulator.GetStones();
            for (StoneId i = (state.current_shot + 1) % 2; i < state.current_shot; i += 2) {  // 相手のショットを列挙
                if (initial_stones[i] && IsStoneInFreeGuardZone(initial_stones[i]->position, stone_radius, shot_side)
                    && !stones[i]) {
                    free_guard_faul = true;
                    break;
                }
            }
        }
        if (free_guard_faul) {
            simulator.SetStones(initial_stones);
        }
    }

    // state.current_shot, state.current_endなどが更新される前に，ショットを行ったチームを記録しておく．
    auto const moved_team = state.GetCurrentTeam();

    // エンド終了時の処理
    if (state.current_shot == 15) {
        // スコア算出
        auto const score = CheckScore(simulator.GetStones(), stone_radius, shot_side, state.current_end_first);

        // スコアを反映
        if (state.current_end < setting.end) {  // 通常エンド
            state.scores[state.current_end] = score;
        } else {  // エクストラエンド
            state.extra_end_score = score;
        }

        // 手番の変更．スコアが0(ブランクエンド)の時は変更されない．
        if (score > 0) {
            state.current_end_first = Team::k1;
        } else if (score < 0) {
            state.current_end_first = Team::k0;
        }

        // ストーン位置のリセット
        for (auto & stone_position : state.stone_positions) {
            stone_position = std::nullopt;
        }

        // エンドとショットカウントを更新
        state.current_shot = 0;
        ++state.current_end;

        // 勝敗の決定
        if (state.current_end >= setting.end) {
            std::int32_t score_sum = 0;
            for (auto score : state.scores) {
                score_sum += score;
            }
            score_sum += state.extra_end_score;

            // 次の手は無い．
            state.current_end_first = Team::kInvalid;

            state.result.emplace();
            if (score_sum > 0) {
                state.result->win = Team::k0;
                state.result->reason = Result::Reason::kScore;
            } else if (score_sum < 0) {
                state.result->win = Team::k1;
                state.result->reason = Result::Reason::kScore;
            } else if (state.current_end >= kExtraEndMax) {  // スコア差無し かつ 延長エンド数限界
                state.result->win = Team::kInvalid;
                state.result->reason = Result::Reason::kInvalid;
            }
        }

    } else {  // エンド中の処理
        auto const& stones = simulator.GetStones();
        for (StoneId i = 0; i < kStoneMax; ++i) {
            if (stones[i]) {
                state.stone_positions[i] = TransformPosition(stones[i]->position, Id::kSimulation, shot_side);
            } else {
                state.stone_positions[i] = std::nullopt;
            }
        }
        ++state.current_shot;
    }

    // コンシードや時間切れの場合はstateを上書きする．
    if (!is_shot) {
        // 次の手は無い．
        state.current_end_first = Team::kInvalid;

        state.result.emplace();
        switch (moved_team) {
            case Team::k0:
                state.result->win = Team::k1;
                break;
            case Team::k1:
                state.result->win = Team::k0;
                break;
            default:
                assert(false);
                break;
        }

        std::visit(
            Overloaded{
                [&state](Concede) {
                    state.result->reason = Result::Reason::kConcede;
                },
                [&state](TimeLimit) {
                    state.result->reason = Result::Reason::kTimeLimit;
                },
                [](auto const&) {
                    assert(false);
                }
            },
            move);
    }
}

} // namespace digital_curling::game::normal
