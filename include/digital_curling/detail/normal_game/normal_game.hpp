#ifndef DIGITAL_CURLING_NORMAL_GAME_NORMAL_GAME_HPP
#define DIGITAL_CURLING_NORMAL_GAME_NORMAL_GAME_HPP

#include <functional>
#include <variant>
#include "../common.hpp"
#include "../simulation/simulator.hpp"

namespace digital_curling::normal_game {



/// <summary>
/// 延長戦を含まないエンド数の最大値
/// </summary>
constexpr std::uint8_t kEndMax = 10;



/// <summary>
/// 延長戦を含めたエンド数の最大値(<see cref="State::current_end"/> >= <see cref="kExtraEndMax"/>のときは無効なエンド)
/// </summary>
constexpr std::uint8_t kExtraEndMax = 255;



/// <summary>
/// チームを識別するために用いる
/// </summary>
enum class TeamId : std::int8_t {
    /// <summary>
    /// チーム0．最初のエンドで先攻のチーム．
    /// </summary>
    k0 = 0,

    /// <summary>
    /// チーム1．最初のエンドで後攻のチーム．
    /// </summary>
    k1 = 1,

    /// <summary>
    /// 無効な値
    /// </summary>
    kInvalid
};

NLOHMANN_JSON_SERIALIZE_ENUM(TeamId, {
    {TeamId::k0, 0},
    {TeamId::k1, 1},
    {TeamId::kInvalid, nullptr},
})



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
    /// ショットの初速に加わる乱数(正規分布をとる)の標準偏差．
    /// </summary>
    float stddev_shot_speed = 0.001f;

    /// <summary>
    /// ショットの初期角度に加わる乱数(正規分布をとる)の標準偏差．
    /// </summary>
    float stddev_shot_angle = 0.001f;

    /// <summary>
    /// シミュレーションのステップごとに呼び出されるコールバックです．
    /// 引数には<see cref="ApplyMove"/>に渡したシミュレータが渡されます．
    /// このコールバックを使用することで，ストーンが動いている最中の情報を得ることができます．
    /// </summary>
    std::function<void(simulation::ISimulator const &)> on_step;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Setting,
    randomize_initial_shot_velocity,
    end,
    sheet_width,
    five_rock_rule,
    max_shot_speed,
    stddev_shot_speed,
    stddev_shot_angle
)



/// <summary>
/// 試合の結果を格納する．
/// </summary>
struct Result {

    /// <summary>
    /// 勝敗が定まった理由
    /// </summary>
    enum class Reason : std::uint8_t {
        /// <summary>スコアによる勝敗．</summary>
        kScore,
        /// <summary>コンシードによる勝敗．</summary>
        kConcede,
        /// <summary>制限時間超過による勝敗．</summary>
        kTimeLimit,
        /// <summary>引き分けの際はこの値が格納される．</summary>
        kInvalid
    };

    /// <summary>
    /// 勝者．引き分けの場合は<see cref="TeamId::kInvalid"/>が格納される．
    /// </summary>
    TeamId win = TeamId::k0;

    /// <summary>
    /// 勝敗の要因．
    /// </summary>
    Reason reason = Reason::kScore;
};

NLOHMANN_JSON_SERIALIZE_ENUM(Result::Reason, {
    {Result::Reason::kScore, "score"},
    {Result::Reason::kConcede, "concede"},
    {Result::Reason::kTimeLimit, "time_limit"},
    {Result::Reason::kInvalid, nullptr},
})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Result,
    win,
    reason
)



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
    /// 現在のエンドの先行．ゲーム終了後は<see cref="TeamId::kInvalid"/>が格納される．
    /// </summary>
    TeamId current_end_first;

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
    /// <param name="team">チームID．<see cref="TeamId::kInvalid"/>は渡さないでください．</param>
    /// <returns>引数で指定したプレイヤーの合計スコア</returns>
    std::uint32_t GetScore(TeamId team) const;

    /// <summary>
    /// 現在ショットを行うチームを得る．ゲームがすでに終了している場合<see cref="TeamId::kInvalid"/>を返す．
    /// </summary>
    /// <returns>現在ショットを行うプレイヤー</returns>
    TeamId GetCurrentTeam() const;
};

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



namespace move {



/// <summary>
/// 行動：ショット
/// </summary>
struct Shot {
    static constexpr char kType[] = "shot";

    /// <summary>
    /// ショットの回転方向
    /// </summary>
    enum class Rotation : std::uint8_t {
        /// <summary>反時計回り</summary>
        kCCW,
        /// <summary>時計回り</summary>
        kCW
    };

    /// <summary>
    /// ショットの初速度
    /// </summary>
    Vector2 velocity;

    /// <summary>
    /// ショットの初期回転方向
    /// </summary>
    Rotation rotation = Rotation::kCCW;
};

NLOHMANN_JSON_SERIALIZE_ENUM(Shot::Rotation, {
    {Shot::Rotation::kCCW, "ccw"},
    {Shot::Rotation::kCW, "cw"},
})



/// <summary>
/// 行動：コンシード
/// </summary>
struct Concede {
    static constexpr char kType[] = "concede";
};



/// <summary>
/// 行動：制限時間超過
/// </summary>
struct TimeLimit {
    static constexpr char kType[] = "time_limit";
};



} // namespace move



/// <summary>
/// ターンごとにチームが採る行動
/// </summary>
using Move = std::variant<move::Shot, move::Concede, move::TimeLimit>;



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



} // namespace digital_curling::normal_game

// ------------------------------------------------------------

namespace nlohmann {

// For serializing/deserializing normal_game::Move to/from json
template <>
struct adl_serializer<digital_curling::normal_game::Move> {
    static void to_json(json & j, digital_curling::normal_game::Move const& m);
    static void from_json(json const& j, digital_curling::normal_game::Move & m);
};

} // namespace nlohmann

#endif // DIGITAL_CURLING_NORMAL_GAME_NORMAL_GAME_HPP
