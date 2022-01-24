#include "digital_curling/detail/game/random/shot_randomizer.hpp"
#include "digital_curling/detail/game/random/random_seed_normal_dist_shot_randomizer.hpp"

namespace digital_curling::game::random {

std::shared_ptr<IShotRandomizer> IShotRandomizer::GetDefault()
{
    static auto shot_randomizer = std::make_shared<RandomSeedNormalDistShotRandomizer>();
    return shot_randomizer;
}

} // namespace digital_curling::game::random
