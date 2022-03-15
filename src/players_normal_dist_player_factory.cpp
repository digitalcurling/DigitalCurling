#include "digital_curling/detail/players/normal_dist_player_factory.hpp"
#include "players_normal_dist_player.hpp"

namespace digital_curling::players {


NormalDistPlayerFactory::NormalDistPlayerFactory(NormalDistPlayerFactory const& rhs)
    : max_speed(rhs.max_speed)
    , stddev_speed(rhs.stddev_speed)
    , stddev_angle(rhs.stddev_angle)
    , seed(rhs.seed)
{}

NormalDistPlayerFactory & NormalDistPlayerFactory::operator = (NormalDistPlayerFactory const& rhs)
{
    if (this == &rhs) return *this;

    max_speed = rhs.max_speed;
    stddev_speed = rhs.stddev_speed;
    stddev_angle = rhs.stddev_angle;
    seed = rhs.seed;

    return *this;
}

std::unique_ptr<IPlayer> NormalDistPlayerFactory::CreatePlayer() const
{
    return std::make_unique<NormalDistPlayer>(*this);
}

std::unique_ptr<IPlayerFactory> NormalDistPlayerFactory::Clone() const
{
    return std::make_unique<NormalDistPlayerFactory>(*this);
}


// json
void to_json(nlohmann::json & j, NormalDistPlayerFactory const& v)
{
    j["type"] = kNormalDistPlayerId;
    j["max_speed"] = v.max_speed;
    j["stddev_speed"] = v.stddev_speed;
    j["stddev_angle"] = v.stddev_angle;
    j["seed"] = v.seed;
}

void from_json(nlohmann::json const& j, NormalDistPlayerFactory & v)
{
    j.at("max_speed").get_to(v.max_speed);
    j.at("stddev_speed").get_to(v.stddev_speed);
    j.at("stddev_angle").get_to(v.stddev_angle);
    j.at("seed").get_to(v.seed);
}

} // namespace digital_curling::players
