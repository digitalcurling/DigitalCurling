// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief C++ と C 構造体間の変換を定義

#pragma once

#include <memory>
#include <string>
#include <variant>
#include <uuidv7/uuidv7.hpp>
#include "digitalcurling/plugins/plugin_api.hpp"
#include "digitalcurling/plugins/plugin_error.hpp"
#include "digitalcurling/simulators/simulator_mode_flag.hpp"
#include "loader_types.h"
#include "plugin_instance_list.hpp"

namespace digitalcurling::plugins {


// --- CTypeConverter ---

/// @brief C++型とC-ABI型の変換を行うコンバータ
/// @tparam CppT C++側の型（例: std::string, Vector2）
/// @tparam CT C-ABI側の型（例: char*, DigitalCurling_Vector2）
/// @details
/// 各型ごとの具体的な変換処理は、この構造体の特殊化として実装されています。
/// 利用者は `CTypeConverter<CppT, CT>::ToCType()` および `FromCType()` を使用してください。
template<typename CppT, typename CT> struct CTypeConverter;


/// @cond Doxygen_Suppress

// --- IsCStruct ---
template<typename T> struct IsCStruct : std::false_type {};
template<> struct IsCStruct<DigitalCurling_Vector2> : std::true_type {};
template<> struct IsCStruct<DigitalCurling_Shot> : std::true_type {};
template<> struct IsCStruct<DigitalCurling_StoneCoordinate> : std::true_type {};

// --- CTypeConverter (Readonly) ---
template <typename T>
struct CTypeConverter<std::nullptr_t, T*> {
    static constexpr bool needs_resolver = false;

    static const T* ToCType(std::nullptr_t) { return nullptr; }
};

//--- CTypeConverter (Readonly) Specializations ---
template<typename CHandleT>
struct CTypeConverter<uuidv7::uuidv7, CHandleT*> {
    static constexpr bool needs_resolver = true;

    static std::shared_ptr<CHandleT> ToCType(PluginInstanceList& map, const uuidv7::uuidv7& id) {
        return map.Get<CHandleT>(id);
    }
};

// --- CTypeConverter (Read/Write) ---
template<typename T>
struct CTypeConverter<T, T> {
    static constexpr bool needs_resolver = false;

    static const T ToCType(const T& value) { return value; }
    static const T FromCType(const T& c_value) { return c_value; }
};
template<typename T>
struct CTypeConverter<T*, T*> {
    static constexpr bool needs_resolver = false;

    static const T* ToCType(const T* value) { return value; }
    static const T* FromCType(const T* c_value) { return c_value; }
};

// --- CTypeConverter (Read/Write) Enum Specializations ---
template<>
struct CTypeConverter<simulators::SimulateModeFlag, DigitalCurling_SimulateModeFlag> {
    static constexpr bool needs_resolver = false;

    static const DigitalCurling_SimulateModeFlag ToCType(const simulators::SimulateModeFlag& value) {
        return static_cast<DigitalCurling_SimulateModeFlag>(value);
    }
    static const simulators::SimulateModeFlag FromCType(const DigitalCurling_SimulateModeFlag& c_value) {
        return static_cast<simulators::SimulateModeFlag>(c_value);
    }
};
template<>
struct CTypeConverter<players::Gender, DigitalCurling_Gender> {
    static constexpr bool needs_resolver = false;

    static const DigitalCurling_Gender ToCType(const players::Gender& value) {
        return static_cast<DigitalCurling_Gender>(value);
    }
    static const players::Gender FromCType(const DigitalCurling_Gender& c_value) {
        return static_cast<players::Gender>(c_value);
    }
};

// --- CTypeConverter (Read/Write) C-struct Specializations ---
template<>
struct CTypeConverter<Vector2, DigitalCurling_Vector2> {
    static constexpr bool needs_resolver = false;

    static const DigitalCurling_Vector2 ToCType(const Vector2& value) {
        return DigitalCurling_Vector2 { value.x, value.y };
    }
    static const Vector2 FromCType(const DigitalCurling_Vector2& c_value) {
        return Vector2 { c_value.x, c_value.y };
    }
};
template<>
struct CTypeConverter<moves::Shot, DigitalCurling_Shot> {
    static constexpr bool needs_resolver = false;

    static const DigitalCurling_Shot ToCType(const moves::Shot& value) {
        return DigitalCurling_Shot {
            value.translational_velocity,
            value.angular_velocity,
            value.release_angle
        };
    }
    static const moves::Shot FromCType(const DigitalCurling_Shot& c_value) {
        return moves::Shot(c_value.translational_velocity, c_value.angular_velocity, c_value.release_angle);
    }
};
template<>
struct CTypeConverter<simulators::ISimulator::AllStones, DigitalCurling_StoneCoordinate> {
    static constexpr bool needs_resolver = false;

    static const DigitalCurling_StoneCoordinate ToCType(const simulators::ISimulator::AllStones& value) {
        auto c_coord = DigitalCurling_StoneCoordinate{};
        for (size_t i = 0; i < value.size(); ++i) {
            if (value[i].has_value()) {
                c_coord.stones[i].position = CTypeConverter<Vector2, DigitalCurling_Vector2>::ToCType(value[i]->position);
                c_coord.stones[i].angle = value[i]->angle;
                c_coord.stones[i].translational_velocity = CTypeConverter<Vector2, DigitalCurling_Vector2>::ToCType(value[i]->translational_velocity);
                c_coord.stones[i].angular_velocity = value[i]->angular_velocity;
            }
        }
        return c_coord;
    }
    static const simulators::ISimulator::AllStones FromCType(const DigitalCurling_StoneCoordinate& c_value) {
        simulators::ISimulator::AllStones stones{};
        for (size_t i = 0; i < stones.size(); ++i) {
            if (c_value.stones[i].angle != 0.f ||
                c_value.stones[i].position.x != 0.f ||
                c_value.stones[i].position.y != 0.f ||
                c_value.stones[i].translational_velocity.x != 0.f ||
                c_value.stones[i].translational_velocity.y != 0.f ||
                c_value.stones[i].angular_velocity != 0.f) {
                stones[i] = simulators::ISimulator::StoneState {
                    CTypeConverter<Vector2, DigitalCurling_Vector2>::FromCType(c_value.stones[i].position),
                    c_value.stones[i].angle,
                    CTypeConverter<Vector2, DigitalCurling_Vector2>::FromCType(c_value.stones[i].translational_velocity),
                    c_value.stones[i].angular_velocity
                };
            }
        }
        return stones;
    }
};

// --- CTypeConverter (Read/Write) Specializations ---
template<>
struct CTypeConverter<std::string, char*> {
    static constexpr bool needs_resolver = false;

    static const char* ToCType(const std::string& value) {
        return value.c_str();
    }
    static const std::string FromCType(const char* c_value) {
        return c_value ? std::string(c_value) : std::string();
    }
};
template<>
struct CTypeConverter<std::string, const char*> : CTypeConverter<std::string, char*> {};

/// @endcond

} // namespace digitalcurling::plugins
