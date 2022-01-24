#ifndef DIGITAL_CURLING_JSON_POLYMORPHIC_CONVERSION_HPP
#define DIGITAL_CURLING_JSON_POLYMORPHIC_CONVERSION_HPP

#include <memory>
#include <map>
#include <string>
#include <typeindex>
#include <type_traits>
#include "nlohmann/json.hpp"

namespace digital_curling::detail {



// 多態性を持つクラスをJSONに変換するための機構
// 参考: https://gamedev.stackexchange.com/a/17759
template <class BaseT>
struct PolymorphicJsonConverter {
private:
    using FromJsonFunc = std::unique_ptr<BaseT>(*)(nlohmann::json const&);
    using ToJsonFunc = void(*)(nlohmann::json &, BaseT const&);
    using FromJsonRegistry = std::map<std::string, FromJsonFunc>;
    using ToJsonRegistry = std::map<std::type_index, ToJsonFunc>;

    static FromJsonRegistry & GetFromJsonRegistry()
    {
        static FromJsonRegistry r;
        return r;
    }

    static ToJsonRegistry & GetToJsonRegistry()
    {
        static ToJsonRegistry r;
        return r;
    }

    template <class T>
    static std::unique_ptr<BaseT> FromJsonTemplate(nlohmann::json const& in)
    {
        auto x = std::make_unique<T>();
        *x = in.get<T>();
        return x;
    }

    template <class T>
    static void ToJsonTemplate(nlohmann::json & out, BaseT const& b)
    {
        out = static_cast<T const&>(b);
    }

public:

    template <class T>
    struct RegistryEntry {
    public:
        static RegistryEntry<T> & Instance(std::string const& type_name)
        {
            static RegistryEntry<T> instance(type_name);
            return instance;
        }
    private:
        RegistryEntry(std::string const& type_name)
        {
            GetFromJsonRegistry().emplace(type_name, FromJsonTemplate<T>);
            GetToJsonRegistry().emplace(typeid(T), ToJsonTemplate<T>);
        }

        static_assert(std::is_base_of_v<BaseT, T>);
    };

    // nlohmann_jsonから呼ばれるto_json関数からはこの関数を呼び出す．
    static void ToJson(nlohmann::json & j, BaseT const& base)
    {
        auto & reg = GetToJsonRegistry();
        auto it = reg.find(typeid(base));
        if (it != reg.end()) {
            it->second(j, base);
        } else {
            throw std::runtime_error("not registered");
        }
    }

    // nlohmann_jsonから呼ばれるfrom_json関数からはこの関数を呼び出す．
    static std::unique_ptr<BaseT> FromJson(nlohmann::json const& j)
    {
        auto type = j["type"].get<std::string>();
        auto & reg = GetFromJsonRegistry();
        auto it = reg.find(type);
        if (it != reg.end()) {
            return it->second(j);
        }
        throw std::runtime_error("not registered");
        return nullptr;  // error
    }
};



} // namespace digital_curling::detail


#define DIGITAL_CURLING_REGISTER_POLYMORPHIC_JSON_CONVERSION(base_type, type, name)                                                                                      \
    namespace {                                                                                                                                        \
        template<class BaseT, class T> class PolymorphicJsonRegistration;                                                                              \
        template<> class PolymorphicJsonRegistration<base_type, type> {                                                                                \
            static ::digital_curling::detail::PolymorphicJsonConverter<base_type>::RegistryEntry<type> const& reg;                                     \
        };                                                                                                                                             \
        ::digital_curling::detail::PolymorphicJsonConverter<base_type>::RegistryEntry<type> const& PolymorphicJsonRegistration<base_type, type>::reg = \
            ::digital_curling::detail::PolymorphicJsonConverter<base_type>::RegistryEntry<type>::Instance(name);                                       \
    }


#endif // DIGITAL_CURLING_JSON_POLYMORPHIC_CONVERSION_HPP
