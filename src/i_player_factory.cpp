#include "digital_curling/detail/i_player_factory.hpp"
#include "digital_curling/detail/players/identical_player_factory.hpp"
#include "digital_curling/detail/players/normal_dist_player_factory.hpp"

namespace digital_curling::polymorphic_json::detail {

template <>
ToJsonRegistry<IPlayerFactory> & GetToJsonRegistry<IPlayerFactory>()
{
    static ToJsonRegistry<IPlayerFactory> registry{
        { typeid(players::IdenticalPlayerFactory), ToJsonFuncTemplate<IPlayerFactory, players::IdenticalPlayerFactory> },
        { typeid(players::NormalDistPlayerFactory), ToJsonFuncTemplate<IPlayerFactory, players::NormalDistPlayerFactory> },
    };
    return registry;
}

template <>
FromJsonRegistry<IPlayerFactory> & GetFromJsonRegistry<IPlayerFactory>()
{
    static FromJsonRegistry<IPlayerFactory> registry{
        { std::string(players::kIdenticalPlayerId), FromJsonFuncTemplate<IPlayerFactory, players::IdenticalPlayerFactory> },
        { std::string(players::kNormalDistPlayerId), FromJsonFuncTemplate<IPlayerFactory, players::NormalDistPlayerFactory> },
    };
    return registry;
}

} // namespace digital_curling::polymorphic_json::detail
