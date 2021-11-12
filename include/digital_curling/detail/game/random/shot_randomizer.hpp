#ifndef DIGITAL_CURLING_GAME_RANDOM_SHOT_RANDOMIZER_HPP
#define DIGITAL_CURLING_GAME_RANDOM_SHOT_RANDOMIZER_HPP

#include <memory>
#include "../../vector2.hpp"
#include "../../json.hpp"

namespace digital_curling::game::random {



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



} // namespace digital_curling::game::random



// json

namespace digital_curling::game::random {

void to_json(nlohmann::json &, IShotRandomizer const&);

} // namespace digital_curling::game::random

namespace nlohmann {

template <>
struct adl_serializer<std::unique_ptr<digital_curling::game::random::IShotRandomizer>> {
    static std::unique_ptr<digital_curling::game::random::IShotRandomizer> from_json(json const&);
    static void to_json(json &, std::unique_ptr<digital_curling::game::random::IShotRandomizer> const&);
};

} // namespace nlohmann

#endif // DIGITAL_CURLING_GAME_RANDOM_SHOT_RANDOMIZER_HPP
