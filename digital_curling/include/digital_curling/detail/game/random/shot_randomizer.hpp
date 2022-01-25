/// \file
/// \brief IShotRandomizer を定義します

#ifndef DIGITAL_CURLING_GAME_RANDOM_SHOT_RANDOMIZER_HPP
#define DIGITAL_CURLING_GAME_RANDOM_SHOT_RANDOMIZER_HPP

#include <memory>
#include "../../vector2.hpp"

namespace digital_curling::game::random {



/// \brief ショットの初速に乱数でブレを与えるためのインターフェイス
class IShotRandomizer {
public:
    virtual ~IShotRandomizer() = default;

    /// \brief ショットの初速にブレを与える．
    ///
    /// この関数はスレッドセーフです．
    /// \param[in] shot_velocity ブレを与えるショットの初速
    /// \returns ブレを与えたショットの初速
    virtual Vector2 Randomize(Vector2 shot_velocity) = 0;

    /// \brief デフォルトのショットランダマイザを得る．
    ///
    /// \returns デフォルトのショットランダマイザ
    static std::shared_ptr<IShotRandomizer> GetDefault();
};



} // namespace digital_curling::game::random

#endif // DIGITAL_CURLING_GAME_RANDOM_SHOT_RANDOMIZER_HPP
