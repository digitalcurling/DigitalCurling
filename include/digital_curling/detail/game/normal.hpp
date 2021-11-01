#ifndef DIGITAL_CURLING_GAME_NORMAL_HPP
#define DIGITAL_CURLING_GAME_NORMAL_HPP

#include "common.hpp"
#include "../simulation/simulator.hpp"

namespace digital_curling::game::normal {

/// <summary>
/// 延長戦を含まないエンド数の最大値
/// </summary>
constexpr std::uint8_t kEndMax = 10;



/// <summary>
/// 延長戦を含めたエンド数の最大値(<see cref="State::current_end"/> >= <see cref="kExtraEndMax"/>のときは無効なエンド)
/// </summary>
constexpr std::uint8_t kExtraEndMax = 255;



/// <summary>
/// 試合設定．
/// </summary>
struct Setting {
    /// <summary>
    /// ショットの初速度に乱数を加える場合true．
    /// </summary>
    bool randomize_initial_shot_velocity = true;

    /// <summary>
    /// 延長戦を含まないエンド数．kEndMax以下．
    /// </summary>
    std::uint8_t end = 10;

    /// <summary>
    /// アイスシートの横幅．
    /// </summary>
    float sheet_width = 4.75f;

    /// <summary>
    /// ファイブロックルールを適用する場合true．
    /// </summary>
    bool five_rock_rule = true;

    /// <summary>
    /// ショットの最大速度
    /// </summary>
    float max_shot_speed = 3.f;

    /// <summary>
    /// ショットの初速にブレを与えるためのもの
    /// </summary>
    std::shared_ptr<IShotRandomizer> shot_randomizer = IShotRandomizer::GetDefault();

    /// <summary>
    /// シミュレーションのステップごとに呼び出されるコールバックです．
    /// 引数には<see cref="ApplyMove"/>に渡したシミュレータが渡されます．
    /// このコールバックを使用することで，ストーンが動いている最中の情報を得ることができます．
    /// </summary>
    std::function<void(simulation::ISimulator const &)> on_step;
};



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
    StoneId current_shot;

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



/// <summary>
/// 行動を適用し，試合を進める．
/// </summary>
/// <remarks>
/// 複数のスレッドで同時に実行する場合は，すべての引数にスレッド毎に作成したインスタンスを指定してください．
/// </remarks>
/// <param name="setting">試合設定(入力)</param>
/// <param name="state">試合状態(入力および出力)</param>
/// <param name="simulator">ストーンの物理シミュレータ(入力および出力)</param>
/// <param name="move">行動(入力および出力)．ショットの場合は乱数が加えられた値を返す．</param>
void ApplyMove(
    Setting const& setting,
    State & state,
    simulation::ISimulator & simulator,
    Move & move);




} // namespace digital_curling::game::normal



// json

namespace digital_curling::game::normal {

void to_json(nlohmann::json &, Setting const&);
void from_json(nlohmann::json const&, Setting &);

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

} // namespace digital_curling::game::normal

#endif // DIGITAL_CURLING_GAME_NORMAL_HPP
