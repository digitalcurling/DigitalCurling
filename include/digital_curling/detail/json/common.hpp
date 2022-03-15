/// \file
/// \brief 必要なJSON変換関数を定義します

#ifndef DIGITAL_CURLING_JSON_COMMON_HPP
#define DIGITAL_CURLING_JSON_COMMON_HPP

#include <optional>
#include <chrono>
#include "nlohmann/json.hpp"
#include "polymorphic_json.hpp"

/// \cond Doxygen_Suppress
// std::optional
namespace nlohmann {
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
/// \endcond


/// \cond Doxygen_Suppress
// std::chrono::milliseconds
namespace nlohmann {
template <>
struct adl_serializer<std::chrono::milliseconds> {
    static void to_json(json & j, std::chrono::milliseconds const& ms)
    {
        j = static_cast<double>(ms.count()) / 1000.0;
    }

    static void from_json(json const& j, std::chrono::milliseconds & ms)
    {
        ms = std::chrono::milliseconds(
            static_cast<std::chrono::milliseconds::rep>(
                std::round(
                    j.get<double>() * 1000.0)));
    }
};
} // namespace nlohmann
/// \endcond

#endif // DIGITAL_CURLING_JSON_COMMON_HPP
