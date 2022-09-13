#include "digital_curling/detail/i_player_storage.hpp"
#include "players_identical_player_storage.hpp"
#include "digital_curling/detail/players/identical_player_factory.hpp"  // for kIdenticalPlayerId
#include "players_normal_dist_player_storage.hpp"

namespace digital_curling::polymorphic_json::detail {

template <>
ToJsonRegistry<IPlayerStorage> & GetToJsonRegistry<IPlayerStorage>()
{
    static ToJsonRegistry<IPlayerStorage> registry{
        { typeid(players::IdenticalPlayerStorage), ToJsonFuncTemplate<IPlayerStorage, players::IdenticalPlayerStorage> },
        { typeid(players::NormalDistPlayerStorage), ToJsonFuncTemplate<IPlayerStorage, players::NormalDistPlayerStorage> },
    };
    return registry;
}

template <>
FromJsonRegistry<IPlayerStorage> & GetFromJsonRegistry<IPlayerStorage>()
{
    static FromJsonRegistry<IPlayerStorage> registry{
        { std::string(players::kIdenticalPlayerId), FromJsonFuncTemplate<IPlayerStorage, players::IdenticalPlayerStorage> },
        { std::string(players::kNormalDistPlayerId), FromJsonFuncTemplate<IPlayerStorage, players::NormalDistPlayerStorage> },
    };
    return registry;
}

} // namespace digital_curling::polymorphic_json::detail
