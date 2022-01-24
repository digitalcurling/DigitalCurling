#ifndef DIGITAL_CURLING_GAME_RANDOM_SHOT_RANDOMIZER_HPP
#define DIGITAL_CURLING_GAME_RANDOM_SHOT_RANDOMIZER_HPP

#include <memory>
#include "../../vector2.hpp"

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
    /// デフォルトのショットランダマイザを得る．
    /// </summary>
    /// <returns>デフォルトのショットランダマイザ</returns>
    static std::shared_ptr<IShotRandomizer> GetDefault();
};



} // namespace digital_curling::game::random

#endif // DIGITAL_CURLING_GAME_RANDOM_SHOT_RANDOMIZER_HPP
