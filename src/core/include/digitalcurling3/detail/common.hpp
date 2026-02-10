// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// \file
/// \brief 必要なJSON変換関数を定義します

#ifndef DIGITALCURLING3_JSON_COMMON_HPP
#define DIGITALCURLING3_JSON_COMMON_HPP

#include <optional>
#include <chrono>
#include "nlohmann/json.hpp"

/// \cond Doxygen_Suppress
namespace {

// overloadedトリック用ヘルパークラス
// 参考: https://dev.to/tmr232/that-overloaded-trick-overloading-lambdas-in-c17
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...)->Overloaded<Ts...>;

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
/// \endcond

#endif // DIGITALCURLING3_JSON_COMMON_HPP
