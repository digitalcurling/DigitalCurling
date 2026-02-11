// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief プレイヤープラグインのエクスポート用マクロを定義

#pragma once

#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#include "digitalcurling/moves/shot.hpp"
#include "digitalcurling/players/i_player.hpp"
#include "digitalcurling/plugins/i_plugin_object.hpp"
#include "digitalcurling/plugins/data_object.h"
#include "digitalcurling/plugins/detail/plugin_exports.hpp"

/// @cond Doxygen_Suppress

namespace digitalcurling::plugins::detail {

// --- Player-specific wrappers ---
template <typename Player>
DigitalCurling_ErrorCode PlayerGetGenderImpl(PlayerHandle* player, DigitalCurling_Gender* out_gender, char** out_error)
{
    if (!player)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "PlayerGetGender: player handle is nullptr.", out_error);
    if (!out_gender)
        return ReturnError(DIGITALCURLING_ERR_BUFFER_NULLPTR, "PlayerGetGender: out_gender is nullptr.", out_error);
    try {
        auto* player_ptr = dynamic_cast<Player*>(player);
        *out_gender = static_cast<DigitalCurling_Gender>(player_ptr->GetGender());
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "PlayerGetGender", out_error);
    }
}

template <typename Player>
DigitalCurling_ErrorCode PlayerPlayImpl(PlayerHandle* player, const DigitalCurling_Shot* shot, DigitalCurling_Shot* out_shot, char** out_error)
{
    if (!player)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "PlayerPlay: player handle is nullptr.", out_error);
    if (!shot)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "PlayerPlay: shot is nullptr.", out_error);
    if (!out_shot)
        return ReturnError(DIGITALCURLING_ERR_BUFFER_NULLPTR, "PlayerPlay: out_shot is nullptr.", out_error);

    try {
        auto* player_ptr = dynamic_cast<Player*>(player);
        auto s = digitalcurling::moves::Shot {
            shot->translational_velocity,
            shot->angular_velocity,
            shot->release_angle
        };
        auto result_shot = player_ptr->Play(s);

        out_shot->translational_velocity = result_shot.translational_velocity;
        out_shot->angular_velocity = result_shot.angular_velocity;
        out_shot->release_angle = result_shot.release_angle;
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "PlayerPlay", out_error);
    }
}


} // namespace

/// @endcond

// --- Export Macros for Users ---
/// @brief プレイヤープラグインのエクスポート用マクロ
#define DIGITALCURLING_EXPORT_PLAYER_PLUGIN(FactoryClass, StorageClass, PlayerClass) \
    static_assert(std::is_base_of_v<digitalcurling::players::IPlayerFactory, FactoryClass>, "FactoryClass must derive from IPlayerFactory"); \
    static_assert(std::is_base_of_v<digitalcurling::players::IPlayerStorage, StorageClass>, "StorageClass must derive from IPlayerStorage"); \
    static_assert(std::is_base_of_v<digitalcurling::players::IPlayer, PlayerClass>, "PlayerClass must derive from IPlayer"); \
    \
    static const digitalcurling::plugins::PlayerApi g_player_api_instance = { \
        /*get_factory*/ &digitalcurling::plugins::detail::GetFactoryImpl<digitalcurling::plugins::PluginType::player, FactoryClass>, \
        /*save*/ &digitalcurling::plugins::detail::TargetSaveImpl<digitalcurling::plugins::PluginType::player, StorageClass>, \
        /*load*/ &digitalcurling::plugins::detail::TargetLoadImpl<digitalcurling::plugins::PluginType::player, StorageClass>, \
        \
        /*get_gender*/ &digitalcurling::plugins::detail::PlayerGetGenderImpl<PlayerClass>, \
        /*play*/ &digitalcurling::plugins::detail::PlayerPlayImpl<PlayerClass> \
    }; \
    DIGITALCURLING_EXPORT_PLUGIN_INNER(digitalcurling::plugins::PluginType::player, FactoryClass, StorageClass, PlayerClass, &g_player_api_instance, nullptr)
