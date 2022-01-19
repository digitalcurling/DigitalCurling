#include "digital_curling/detail/vector2.hpp"

namespace digital_curling {

void to_json(nlohmann::json & j, Vector2 const& v)
{
    j[0] = v.x;
    j[1] = v.y;
}

void from_json(nlohmann::json const& j, Vector2 & v)
{
    v.x = j.at(0);
    v.y = j.at(1);
}

} // namespace digital_curling
