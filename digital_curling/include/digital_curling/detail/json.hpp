#ifndef DIGITAL_CURLING_JSON_HPP
#define DIGITAL_CURLING_JSON_HPP

#include <optional>
#include "nlohmann/json.hpp"

// json

namespace nlohmann {

// For serializing/deserializing std::optional to/from json
template <typename T>
struct adl_serializer<std::optional<T>> {
    static void to_json(json & j, std::optional<T> const& opt)
    {
        if (opt) {
            j = *opt;
        } else {
            j = nullptr;
        }
    }

    static void from_json(json const& j, std::optional<T> & opt)
    {
        if (j.is_null()) {
            opt = std::nullopt;
        } else {
            opt = j.get<T>();
        }
    }
};

} // namespace nlohmann

#endif // DIGITAL_CURLING_JSON_HPP
