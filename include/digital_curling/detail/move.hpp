/// \file
/// \brief Move を定義します

#ifndef DIGITAL_CURLING_MOVE_HPP
#define DIGITAL_CURLING_MOVE_HPP

#include <variant>
#include "moves/shot.hpp"
#include "moves/concede.hpp"

namespace digital_curling {



/// \brief ターンごとにチームが採る行動
///
/// \sa moves::Shot, moves::Concede
using Move = std::variant<moves::Shot, moves::Concede>;



} // namespace digital_curling


/// \cond Doxygen_Suppress
// json
namespace nlohmann {
template <>
struct adl_serializer<digital_curling::Move> {
    static void to_json(json & j, digital_curling::Move const& m);
    static void from_json(json const& j, digital_curling::Move & m);
};
} // namespace nlohmann
/// \endcond


#endif // DIGITAL_CURLING_MOVE_HPP
