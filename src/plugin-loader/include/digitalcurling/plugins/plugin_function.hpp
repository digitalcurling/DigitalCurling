// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief プラグイン関数ラッパーテンプレートを定義

#pragma once

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include "digitalcurling/plugins/plugin_api.hpp"
#include "digitalcurling/plugins/plugin_error.hpp"
#include "digitalcurling/plugins/c_type_converter.hpp"

#if !defined(_WIN32) && !defined(__cdecl)
    #define __cdecl
#endif

namespace digitalcurling::plugins {


// --- CFunctionTraits ---
template <typename T> struct StripCp { using type = T; };
template <typename T> struct StripCp<const T> : StripCp<T> {};
template <typename T> struct StripCp<T*> : StripCp<T> {};
template <typename T> using strip_cp_t = typename StripCp<T>::type;

template <typename Tuple, typename Indices>
struct TupleTakeImpl;
template <typename Tuple, std::size_t... Is>
struct TupleTakeImpl<Tuple, std::index_sequence<Is...>> {
    using type = std::tuple<std::tuple_element_t<Is, Tuple>...>;
};

template<bool IsVoid, typename F>
struct CFunctionTraits {
    static_assert(!sizeof(F*), "Unsupported function pointer type for CFunctionTraits");
};
template<bool IsVoid, typename... Args>
struct CFunctionTraits<IsVoid, DigitalCurling_ErrorCode (__cdecl *)(Args...)>
{
    using FullArgTuple = std::tuple<Args...>;
    static constexpr size_t NumArgs = sizeof...(Args);
    static_assert(
        std::is_same_v<std::tuple_element_t<NumArgs - 1, FullArgTuple>, char**>,
        "Invalid function: Last argument must be 'char**'."
    );

    using ArgTuple = typename TupleTakeImpl<
        FullArgTuple,
        std::make_index_sequence<NumArgs - (IsVoid ? 1 : 2)>
    >::type;

    using ResultType = std::conditional_t<
        IsVoid, void,
        std::remove_pointer_t<std::tuple_element_t<NumArgs - 2, FullArgTuple>>
    >;
};

// --- is_complete_type ---
template <typename T, typename = void>
struct is_complete_type : std::false_type {};
template <typename T>
struct is_complete_type<T, std::void_t<decltype(sizeof(T))>> : std::true_type {};

// --- IsSmartPtr ---
template<typename T> struct IsSmartPtr : std::false_type {};
template<typename T> struct IsSmartPtr<std::unique_ptr<T>> : std::true_type {};
template<typename T> struct IsSmartPtr<std::shared_ptr<T>> : std::true_type {};


/// @brief 値または plugin_error のいずれかを保持する結果型
template<typename T>
class [[nodiscard]] PluginFunctionResult {
public:
    PluginFunctionResult(T value) : result_(std::move(value)) {}
    PluginFunctionResult(plugin_error error) : result_(std::move(error)) {}

    // State checking
    operator bool() const { return std::holds_alternative<T>(result_); }
    bool HasValue() const { return std::holds_alternative<T>(result_); }

    // Value access
    const T& GetValue() const& {
        if (!HasValue()) throw std::logic_error("Result contains an error: " + std::string(GetError().what()));
        return std::get<T>(result_);
    }
    T& GetValue() & {
        if (!HasValue()) throw std::logic_error("Result contains an error: " + std::string(GetError().what()));
        return std::get<T>(result_);
    }
    const T&& GetValue() const&& {
        if (!HasValue()) throw std::logic_error("Result contains an error: " + std::string(GetError().what()));
        return std::move(std::get<T>(result_));
    }
    T&& GetValue() && {
        if (!HasValue()) throw std::logic_error("Result contains an error: " + std::string(GetError().what()));
        return std::move(std::get<T>(result_));
    }

    // Error access
    const plugin_error& GetError() const {
        if (HasValue()) throw std::logic_error("Result does not contain an error");
        return std::get<plugin_error>(result_);
    }

private:
    std::variant<T, plugin_error> result_;
};

template<>
class [[nodiscard]] PluginFunctionResult<void> {
public:
    PluginFunctionResult() : error_(std::nullopt) {}
    PluginFunctionResult(plugin_error error) : error_(std::move(error)) {}

    operator bool() const { return !error_.has_value(); }
    bool HasValue() const { return !error_.has_value(); }

    const plugin_error& GetError() const {
        if (HasValue()) throw std::logic_error("Result does not contain an error");
        return error_.value();
    }

private:
    std::optional<plugin_error> error_;
};


class PluginFunctionBase {
public:
    virtual explicit operator bool() const = 0;

protected:
    PluginFunctionBase(FreeStringFunc free_string_func, PluginInstanceList& instance_list)
        : free_string_func_(free_string_func), instance_list_(instance_list) {}

    FreeStringFunc free_string_func_;
    PluginInstanceList& instance_list_;

    template<typename T>
    auto GetCArg(T& arg) const {
        using DecayedT = std::decay_t<T>;

        if constexpr (IsCStruct<DecayedT>::value) {
            return &arg;
        } else if constexpr (IsSmartPtr<DecayedT>::value) {
            return arg.get();
        } else {
            return arg;
        }
    }

    inline std::string GetStringAndFree(char* buffer) const {
        if (!buffer) return {};

        std::string message(buffer);
        if (free_string_func_) free_string_func_(buffer);
        return message;
    }
    inline std::string GetErrorMessageAndFree(char* error_buffer) const {
        if (!error_buffer)
            return "Plugin execution failed with no detailed error message.";

        return GetStringAndFree(error_buffer);
    }

    template<bool IsCpp, typename CArgTuple, typename CppArgTuple, size_t... Is>
    auto ConvertTuple(CppArgTuple&& cpp_tuple, std::index_sequence<Is...>) const
    {
        auto converter = ConvertFunctor<IsCpp, CArgTuple, CppArgTuple>{ instance_list_, cpp_tuple };
        return std::make_tuple(
            converter(std::integral_constant<size_t, Is>{})...
        );
    }

    inline std::optional<plugin_error> CheckValid(void* func_ptr) const {
        if (!func_ptr) {
            return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Function pointer is null."};
        }
        if (!free_string_func_) {
            return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Free string function pointer is null."};
        }
        return std::nullopt;
    }

    template <bool IsCpp, typename CArgTuple, typename CppTupleRef>
    struct ConvertFunctor {
        PluginInstanceList& instance_list;
        CppTupleRef cpp_tuple;

        template <typename T>
        using converter_t = std::conditional_t<
            IsCStruct<strip_cp_t<T>>::value, strip_cp_t<T>, std::decay_t<T>
        >;

        template <size_t I>
        auto operator()(std::integral_constant<size_t, I>) {
            using CppArg = std::tuple_element_t<I, std::decay_t<CppTupleRef>>;
            using CArg = std::tuple_element_t<I, CArgTuple>;
            using Converter = CTypeConverter<converter_t<CppArg>, converter_t<CArg>>;

            if constexpr (Converter::needs_resolver) {
                auto ptr = Converter::ToCType(instance_list, std::get<I>(cpp_tuple));
                if (ptr) return ptr;

                throw plugin_error{
                    DIGITALCURLING_ERR_INVALID_PLUGIN_TYPE,
                    "Failed to convert argument at index " + std::to_string(I)
                };
            } else if constexpr (IsCpp) {
                return Converter::ToCType(std::get<I>(cpp_tuple));
            } else {
                return std::get<I>(cpp_tuple);
            }
        }
    };
};

// Return value function
template<typename FuncPtr, typename CppReturnType, typename = void>
class PluginFunction : public PluginFunctionBase {
    using Traits = CFunctionTraits<false, FuncPtr>;
    using CArgTuple = typename Traits::ArgTuple;
    using CResultType = typename Traits::ResultType;

public:
    PluginFunction(FuncPtr func_ptr, FreeStringFunc free, PluginInstanceList& list)
        : PluginFunctionBase(free, list), func_ptr_(func_ptr) {}

    virtual explicit operator bool() const {
        return func_ptr_ != nullptr && free_string_func_ != nullptr;
    }

    template<typename... CppArgs>
    CppReturnType Execute(CppArgs&&... cpp_args) const {
        auto res = ExecuteImpl<true>(std::forward<CppArgs>(cpp_args)...);
        if (!res) throw res.GetError();
        return CTypeConverter<CppReturnType, CResultType>::FromCType(res.GetValue());
    }
    template<typename... CppArgs>
    PluginFunctionResult<CResultType> ExecuteRaw(CppArgs&&... cpp_args) const {
        try {
            return ExecuteImpl<false>(std::forward<CppArgs>(cpp_args)...);
        } catch (const plugin_error& err) {
            return err;
        } catch (...) {
            return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Unknown error occurred in function."};
        }
    }

private:
    FuncPtr func_ptr_;

    template<bool IsCpp, typename... CppArgs>
    PluginFunctionResult<CResultType> ExecuteImpl(CppArgs&&... cpp_args) const {
        if (auto err = CheckValid(reinterpret_cast<void*>(func_ptr_)); err)
            return err.value();

        auto cpp_tuple = std::make_tuple(std::forward<CppArgs>(cpp_args)...);
        auto c_args_tuple = ConvertTuple<IsCpp, CArgTuple>(
            cpp_tuple, std::make_index_sequence<sizeof...(CppArgs)>{}
        );

        CResultType c_result{};
        char* error_buffer = nullptr;
        DigitalCurling_ErrorCode err = std::apply(
            [&](auto&... c_args) { return func_ptr_(GetCArg(c_args)..., &c_result, &error_buffer); },
            c_args_tuple
        );

        if (err != DIGITALCURLING_OK)
            return plugin_error { err, GetErrorMessageAndFree(error_buffer) };
        if (error_buffer) free_string_func_(error_buffer);
        return c_result;
    }
};

// Return value function specialization for char*
template<typename FuncPtr, typename CppReturnType>
class PluginFunction<
    FuncPtr,
    CppReturnType,
    std::enable_if_t<
        is_complete_type<CTypeConverter<CppReturnType, char*>>::value &&
        std::is_same_v<typename CFunctionTraits<false, FuncPtr>::ResultType, char*>
    >
> : public PluginFunctionBase {
    using Traits = CFunctionTraits<false, FuncPtr>;
    using CArgTuple = typename Traits::ArgTuple;
    static_assert(std::is_same_v<typename Traits::ResultType, char*>, "CResultType must be char* for this specialization.");

public:
    PluginFunction(FuncPtr func_ptr, FreeStringFunc free, PluginInstanceList& list)
        : PluginFunctionBase(free, list), func_ptr_(func_ptr) {}

    virtual explicit operator bool() const {
        return func_ptr_ != nullptr && free_string_func_ != nullptr;
    }

    template<typename... CppArgs>
    CppReturnType Execute(CppArgs&&... cpp_args) const {
        auto res = ExecuteImpl<true>(std::forward<CppArgs>(cpp_args)...);
        if (!res) throw res.GetError();

        auto ptr = res.GetValue();
        if (!ptr) throw plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Result string is null."};

        auto ret = CTypeConverter<CppReturnType, char*>::FromCType(ptr);
        free_string_func_(ptr);
        return ret;
    }
    template<typename... CppArgs>
    PluginFunctionResult<std::string> ExecuteRaw(CppArgs&&... cpp_args) const {
        try {
            auto res = ExecuteImpl<false>(std::forward<CppArgs>(cpp_args)...);
            if (!res) return res.GetError();

            auto ptr = res.GetValue();
            if (!ptr) return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Result string is null."};

            auto str = CTypeConverter<std::string, char*>::FromCType(ptr);
            free_string_func_(ptr);
            return str;
        } catch (const plugin_error& err) {
            return err;
        } catch (...) {
            return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Unknown error occurred in function."};
        }
    }

private:
    FuncPtr func_ptr_;

    template<bool IsCpp, typename... CppArgs>
    PluginFunctionResult<char*> ExecuteImpl(CppArgs&&... cpp_args) const {
        if (auto err = CheckValid(reinterpret_cast<void*>(func_ptr_)); err)
            return err.value();

        auto cpp_tuple = std::make_tuple(std::forward<CppArgs>(cpp_args)...);
        auto c_args_tuple = ConvertTuple<IsCpp, CArgTuple>(
            cpp_tuple, std::make_index_sequence<sizeof...(CppArgs)>{}
        );

        char* c_result{};
        char* error_buffer = nullptr;
        DigitalCurling_ErrorCode err = std::apply(
            [&](auto&... c_args) { return func_ptr_(GetCArg(c_args)..., &c_result, &error_buffer); },
            c_args_tuple
        );

        if (err != DIGITALCURLING_OK) {
            if (c_result) this->free_string_func_(c_result);
            return plugin_error { err, GetErrorMessageAndFree(error_buffer) };
        }
        if (error_buffer) free_string_func_(error_buffer);
        return c_result;
    }
};

// Void return function specialization
template<typename FuncPtr>
class PluginFunction<FuncPtr, void, void> : public PluginFunctionBase {
    using Traits = CFunctionTraits<true, FuncPtr>;
    using CArgTuple = typename Traits::ArgTuple;

public:
    PluginFunction(FuncPtr func_ptr, FreeStringFunc free, PluginInstanceList& list)
        : PluginFunctionBase(free, list), func_ptr_(func_ptr) {}

    virtual explicit operator bool() const {
        return func_ptr_ != nullptr && free_string_func_ != nullptr;
    }

    template<typename... CppArgs>
    void Execute(CppArgs&&... cpp_args) const {
        auto res = ExecuteImpl<true>(std::forward<CppArgs>(cpp_args)...);
        if (!res) throw res.GetError();
    }
    template<typename... CppArgs>
    PluginFunctionResult<void> ExecuteRaw(CppArgs&&... cpp_args) const {
        try {
            return ExecuteImpl<false>(std::forward<CppArgs>(cpp_args)...);
        } catch (const plugin_error& err) {
            return err;
        } catch (...) {
            return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Unknown error occurred in function."};
        }
    }

private:
    FuncPtr func_ptr_;

    template<bool IsCpp, typename... CppArgs>
    PluginFunctionResult<void> ExecuteImpl(CppArgs&&... cpp_args) const {
        if (auto err = CheckValid(reinterpret_cast<void*>(func_ptr_)); err)
            return err.value();

        auto cpp_tuple = std::make_tuple(std::forward<CppArgs>(cpp_args)...);
        auto c_args_tuple = ConvertTuple<IsCpp, CArgTuple>(
            cpp_tuple, std::make_index_sequence<sizeof...(CppArgs)>{}
        );

        char* error_buffer = nullptr;
        DigitalCurling_ErrorCode err = std::apply(
            [&](auto&... c_args) { return func_ptr_(GetCArg(c_args)..., &error_buffer); },
            c_args_tuple
        );

        if (err != DIGITALCURLING_OK)
            return plugin_error { err, GetErrorMessageAndFree(error_buffer) };

        if (error_buffer) free_string_func_(error_buffer);
        return {};
    }
};

// Creator function specialization
template<typename FuncPtr>
class PluginFunction<FuncPtr, uuidv7::uuidv7, void> : public PluginFunctionBase {
    using Traits = CFunctionTraits<false, FuncPtr>;
    using CArgTuple = typename Traits::ArgTuple;
    using CResultType = typename Traits::ResultType;

public:
    PluginFunction(FuncPtr func_ptr, FreeStringFunc free, DestroyObjectFunc destroy, PluginInstanceList& list)
        : PluginFunctionBase(free, list), func_ptr_(func_ptr), destroy_obj_func_(destroy) {}

    virtual explicit operator bool() const {
        return func_ptr_ != nullptr && free_string_func_ != nullptr && destroy_obj_func_ != nullptr;
    }

    template<typename... CppArgs>
    uuidv7::uuidv7 Execute(CppArgs&&... cpp_args) const {
        auto res = ExecuteImpl<true>(std::forward<CppArgs>(cpp_args)...);
        if (!res) throw res.GetError();
        return res.GetValue();
    }
    template<typename... CppArgs>
    PluginFunctionResult<uuidv7::uuidv7> ExecuteRaw(CppArgs&&... cpp_args) const {
        try {
            return ExecuteImpl<false>(std::forward<CppArgs>(cpp_args)...);
        } catch (const plugin_error& err) {
            return err;
        } catch (...) {
            return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Unknown error occurred in function."};
        }
    }

private:
    FuncPtr func_ptr_;
    DestroyObjectFunc destroy_obj_func_;

    template<bool IsCpp, typename... CppArgs>
    PluginFunctionResult<uuidv7::uuidv7> ExecuteImpl(CppArgs&&... cpp_args) const {
        if (auto err = CheckValid(reinterpret_cast<void*>(func_ptr_)); err)
            return err.value();
        if (!destroy_obj_func_)
            return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Destroy object function pointer is null."};

        auto cpp_tuple = std::make_tuple(std::forward<CppArgs>(cpp_args)...);
        auto c_args_tuple = ConvertTuple<IsCpp, CArgTuple>(
            cpp_tuple, std::make_index_sequence<sizeof...(CppArgs)>{}
        );

        CResultType c_result{};
        char* error_buffer = nullptr;
        DigitalCurling_ErrorCode err = std::apply(
            [&](auto&... c_args) { return func_ptr_(GetCArg(c_args)..., &c_result, &error_buffer); },
            c_args_tuple
        );

        if (err != DIGITALCURLING_OK) {
            if (c_result) destroy_obj_func_(c_result);
            return plugin_error { err, GetErrorMessageAndFree(error_buffer) };
        }
        if (error_buffer) free_string_func_(error_buffer);

        if (!c_result)
            return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Plugin function returned null handle."};

        using HandleType = std::remove_pointer_t<CResultType>;
        auto res = instance_list_.Add(std::shared_ptr<HandleType>(c_result, destroy_obj_func_));
        if (!res.has_value()) {
            destroy_obj_func_(c_result);
            return plugin_error{DIGITALCURLING_ERR_UNKNOWN, "Failed to register created instance."};
        }
        return res.value();
    }
};

} // namespace digitalcurling::plugins
