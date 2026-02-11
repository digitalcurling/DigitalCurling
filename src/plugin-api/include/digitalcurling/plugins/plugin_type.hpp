// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief プラグインの種類と関連する型を定義

#pragma once

#include <cassert>
#include "digitalcurling/players/i_player.hpp"
#include "digitalcurling/simulators/i_simulator.hpp"

namespace digitalcurling {

namespace plugins {

class PlayerPluginResource;
class SimulatorPluginResource;

/// @brief プラグインの種類を表す列挙型
enum class PluginType {
    /// @brief シミュレーター
    simulator = 0,
    /// @brief プレイヤー
    player = 1,
    /// @brief 無効なプラグインタイプ
    invalid = -1,
};


/// @cond Doxygen_Suppress
template<typename T>
inline constexpr bool always_false = false;
/// @endcond

/// @brief プラグインの種類に対応する型情報を取得するトレイト
template <PluginType Type> struct PluginTypeTraits {
    static_assert(always_false<Type>, "PluginTypeTraits is not defined for this PluginType");

    /// @brief ターゲットの型
    using TargetType = void;
    /// @brief ファクトリーの型
    using FactoryType = void;
    /// @brief ストレージの型
    using StorageType = void;
    /// @brief リソースの型
    using ResourceType = void;
    /// @brief ハンドルの型
    using HandleType = void;
};

/// @cond Doxygen_Suppress
template <> struct PluginTypeTraits<PluginType::simulator> {
    using TargetType = simulators::ISimulator;
    using FactoryType = simulators::ISimulatorFactory;
    using StorageType = simulators::ISimulatorStorage;
    using ResourceType = SimulatorPluginResource;
    using HandleType = SimulatorHandle;
};
template <> struct PluginTypeTraits<PluginType::player> {
    using TargetType = players::IPlayer;
    using FactoryType = players::IPlayerFactory;
    using StorageType = players::IPlayerStorage;
    using ResourceType = PlayerPluginResource;
    using HandleType = PlayerHandle;
};
/// @endcond

/// @brief ターゲットの型に対応する PluginType を取得するトレイト
template <typename T> struct PluginTypeFor {
    static_assert(always_false<T>, "PluginTypeFor is not defined for this type");

    /// @brief 対応する PluginType
    static constexpr PluginType value = PluginType::invalid;
};

/// @cond Doxygen_Suppress
#define REGISTER_PLUGIN_TYPE_MAPPING(TypeVal) \
    template <> struct PluginTypeFor<typename PluginTypeTraits<TypeVal>::TargetType> { static constexpr PluginType value = TypeVal; }; \
    template <> struct PluginTypeFor<typename PluginTypeTraits<TypeVal>::FactoryType> { static constexpr PluginType value = TypeVal; }; \
    template <> struct PluginTypeFor<typename PluginTypeTraits<TypeVal>::StorageType> { static constexpr PluginType value = TypeVal; };

REGISTER_PLUGIN_TYPE_MAPPING(PluginType::simulator)
REGISTER_PLUGIN_TYPE_MAPPING(PluginType::player)

#undef REGISTER_PLUGIN_TYPE_MAPPING
/// @endcond

} // namespace digitalcurling::plugins

/// @brief PluginType を文字列に変換する
/// @param type 変換する PluginType
/// @param capitalize 先頭文字を大文字にするかどうか
/// @return 変換された文字列
inline std::string ToString(plugins::PluginType type, bool capitalize = false) {
    switch (type) {
        case plugins::PluginType::simulator: return capitalize ? "Simulator" : "simulator";
        case plugins::PluginType::player: return capitalize ? "Player" : "player";
        default:
            assert(false && "Invalid PluginType");
            return capitalize ? "Unknown" : "unknown";
    }
};

} // namespace digitalcurling
