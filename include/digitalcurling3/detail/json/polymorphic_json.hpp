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
/// \brief 多態性を持つクラスのJSON変換のための仕組みを提供します

#ifndef DIGITALCURLING3_JSON_POLYMORPHIC_JSON_HPP
#define DIGITALCURLING3_JSON_POLYMORPHIC_JSON_HPP

#include <memory>
#include <unordered_map>
#include <string>
#include <typeindex>
#include <type_traits>
#include <sstream>
#include "nlohmann/json.hpp"

namespace digitalcurling3::polymorphic_json {

/// \cond Doxygen_Suppress
namespace detail {

template <class BaseT>
using ToJsonFunc = void (*)(nlohmann::json &, BaseT const&);

template <class BaseT>
using ToJsonRegistry = std::map<std::type_index, ToJsonFunc<BaseT>>;

template <class BaseT>
using FromJsonFunc = std::unique_ptr<BaseT>(*)(nlohmann::json const&);

template <class BaseT>
using FromJsonRegistry = std::map<std::string, FromJsonFunc<BaseT>>;

template <class BaseT>
ToJsonRegistry<BaseT> & GetToJsonRegistry()
{
    static ToJsonRegistry<BaseT> map;
    return map;
}

template <class BaseT>
FromJsonRegistry<BaseT> & GetFromJsonRegistry()
{
    static FromJsonRegistry<BaseT> map;
    return map;
}

template <class BaseT, class T>
void ToJsonFuncTemplate(nlohmann::json & j, BaseT const& v)
{
    static_assert(std::is_base_of_v<BaseT, T>);
    j = static_cast<T const&>(v);
}

template <class BaseT, class T>
std::unique_ptr<BaseT> FromJsonFuncTemplate(nlohmann::json const& j)
{
    static_assert(std::is_base_of_v<BaseT, T>);
    return std::make_unique<T>(j.get<T>());
}


template <class BaseT>
struct Serializer {

    static void to_json(nlohmann::json & j, BaseT const& v)
    {
        auto const& registry = ::digitalcurling3::polymorphic_json::detail::GetToJsonRegistry<BaseT>();
        auto it = registry.find(typeid(v));
        if (it != registry.end()) {
            it->second(j, v);
        } else {
            std::ostringstream buf;
            buf << "polymorphic json: type \"" << typeid(v).name() << "\" is not registered to \"" << typeid(BaseT).name() << "\"";
            throw std::runtime_error(buf.str());
        }
    }
};

template <class BaseT>
struct Serializer<std::unique_ptr<BaseT>> {

    static void to_json(nlohmann::json & j, std::unique_ptr<BaseT> const& v)
    {
        if (v) {
            Serializer<BaseT>::to_json(j, *v);
        } else {
            j = nullptr;
        }
    }

    static void from_json(nlohmann::json const& j, std::unique_ptr<BaseT> & v)
    {
        if (j.is_null()) {
            v = nullptr;
            return;
        }

        auto const name = j.at("type").get<std::string>();
        auto const& registry = ::digitalcurling3::polymorphic_json::detail::GetFromJsonRegistry<BaseT>();
        auto it = registry.find(name);
        if (it != registry.end()) {
            v = it->second(j);
        } else {
            std::ostringstream buf;
            buf << "polymorphic json: type name \"" << name << "\" is not registered to \"" << typeid(BaseT).name() << "\"";
            throw std::runtime_error(buf.str());
        }
    }
};

} // namespace detail
/// \endcond



/// \brief 多態性を考慮したJSON変換の登録をします
/// \tparam BaseT 登録先の基底クラス(インターフェース)
/// \tparam T 登録する \p BaseT を継承したクラス
/// \param name 型を識別するための文字列．JSONオブジェクトの "type" に該当する．
template <class BaseT, class T>
void Register(std::string_view name)
{
    static_assert(std::is_base_of_v<BaseT, T>);
    ::digitalcurling3::polymorphic_json::detail::GetToJsonRegistry<BaseT>().emplace(typeid(T), ::digitalcurling3::polymorphic_json::detail::ToJsonFuncTemplate<BaseT, T>);
    ::digitalcurling3::polymorphic_json::detail::GetFromJsonRegistry<BaseT>().emplace(std::string(name), ::digitalcurling3::polymorphic_json::detail::FromJsonFuncTemplate<BaseT, T>);
}


} // namespace digitalcurling3::polymorphic_json

#endif // DIGITALCURLING3_JSON_POLYMORPHIC_JSON_HPP
