// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief JSON変換関数を定義

#pragma once

#include <optional>
#include <chrono>
#include <string>
#include <nlohmann/json.hpp>

/// @cond Doxygen_Suppress
namespace {

// overloadedトリック用ヘルパークラス
// 参考: https://dev.to/tmr232/that-overloaded-trick-overloading-lambdas-in-c17
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...)->Overloaded<Ts...>;

template <typename T> struct type_identity { using type = T; };
template <typename T> using type_identity_t = typename type_identity<T>::type;

template<typename T>
inline T try_get_to(nlohmann::json const& j, std::string const& key, T &target, type_identity_t<T> default_value)
{
    if (j.contains(key)) {
        return j.at(key).get_to(target);
    } else {
        target = default_value;
        return default_value;
    }
}

} // unnamed namespace


namespace nlohmann {

// std::optional
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

// std::chrono::milliseconds
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
/// @endcond
