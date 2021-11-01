#ifndef DIGITAL_CURLING_GAME_COMMON_HPP
#define DIGITAL_CURLING_GAME_COMMON_HPP

#include <variant>
#include "../common.hpp"

namespace digital_curling::game {

/// <summary>
/// チームを識別するために用いる
/// </summary>
enum class Team : std::int8_t {
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

NLOHMANN_JSON_SERIALIZE_ENUM(Team, {
    {Team::k0, 0},
    {Team::k1, 1},
    {Team::kInvalid, nullptr},
})



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
    /// 勝者．引き分けの場合は<see cref="Team::kInvalid"/>が格納される．
    /// </summary>
    Team win = Team::k0;

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



/// <summary>
/// ターンごとにチームが採る行動
/// </summary>
using Move = std::variant<Shot, Concede, TimeLimit>;



/// <summary>
/// ショットの初速に乱数でブレを与えるためのインターフェイス
/// </summary>
class IShotRandomizer {
public:
    virtual ~IShotRandomizer() = default;

    /// <summary>
    /// ショットの初速にブレを与える．
    /// </summary>
    /// <remarks>
    /// この関数はスレッドセーフです．
    /// </remarks>
    /// <param name="shot_velocity">ブレを与えるショットの初速</param>
    /// <returns>ブレを与えたショットの初速</returns>
    virtual Vector2 Randomize(Vector2 shot_velocity) = 0;

    /// <summary>
    /// JSONに変換する．
    /// </summary>
    /// <param name="j">出力先</param>
    virtual void ToJson(nlohmann::json & j) const = 0;

    /// <summary>
    /// デフォルトのショットランダマイザを得る．
    /// </summary>
    /// <returns>デフォルトのショットランダマイザ</returns>
    static std::shared_ptr<IShotRandomizer> GetDefault();
};

void to_json(nlohmann::json &, IShotRandomizer const&);

} // namespace digital_curling::game



namespace nlohmann {

// For serializing/deserializing normal_game::Move to/from json
template <>
struct adl_serializer<digital_curling::game::Move> {
    static void to_json(json & j, digital_curling::game::Move const& m);
    static void from_json(json const& j, digital_curling::game::Move & m);
};

template <>
struct adl_serializer<std::unique_ptr<digital_curling::game::IShotRandomizer>> {
    static std::unique_ptr<digital_curling::game::IShotRandomizer> from_json(json const&);
    static void to_json(json &, std::unique_ptr<digital_curling::game::IShotRandomizer> const&);
};

} // namespace nlohmann

#endif // DIGITAL_CURLING_GAME_COMMON_HPP
