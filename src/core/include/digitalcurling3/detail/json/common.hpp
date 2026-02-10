// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file
/// \brief 必要なJSON変換関数を定義します

#ifndef DIGITALCURLING3_JSON_COMMON_HPP
#define DIGITALCURLING3_JSON_COMMON_HPP

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

#endif // DIGITALCURLING3_JSON_COMMON_HPP
