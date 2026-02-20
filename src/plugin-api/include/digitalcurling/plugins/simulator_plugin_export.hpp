// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief プラグインのエクスポート用マクロを定義

#pragma once

#include <cmath>
#include <exception>
#include <optional>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "digitalcurling/moves/shot.hpp"
#include "digitalcurling/plugins/i_plugin_object.hpp"
#include "digitalcurling/simulators/i_simulator.hpp"
#include "digitalcurling/plugins/data_object.h"
#include "digitalcurling/plugins/plugin_api.hpp"
#include "digitalcurling/plugins/detail/plugin_exports.hpp"


/// @cond Doxygen_Suppress
namespace digitalcurling::plugins::detail {


// --- Simulator-specific wrappers ---
template <typename Simulator>
void SimulatorSimulateLoopBody(Simulator* sim, const DigitalCurling_SimulateModeFlag mode_flag, const int frames, const float sheet_width)
{
    const float x_limit = sheet_width / 2.0f - digitalcurling::Stone::kRadius;
    constexpr float y_limit = digitalcurling::coordinate::kBackBoardY - digitalcurling::Stone::kRadius;

    const auto is_out_of_sheet = [x_limit, y_limit](std::optional<digitalcurling::simulators::ISimulator::StoneState> const& stone) {
        if (!stone.has_value()) return false;
        return std::abs(stone->position.x) > x_limit || stone->position.y > y_limit || stone->position.y < 0;
    };

    int f = frames;
    while (!sim->AreAllStonesStopped()) {
        sim->Step();

        if (sheet_width > 0.f) {
            bool is_stone_out = false;
            const auto& stones = sim->GetStones();
            for (const auto& stone : stones) {
                if (is_out_of_sheet(stone)) {
                    is_stone_out = true;
                    break;
                }
            }

            if (is_stone_out) {
                digitalcurling::simulators::ISimulator::AllStones new_stones;
                for (int i = 0; i < digitalcurling::StoneCoordinate::kStoneMax; i++) {
                    const auto& stone = stones[i];
                    if (!is_out_of_sheet(stone)) {
                        new_stones[i] = stone;
                    }
                }
                sim->SetStones(new_stones);

                if (mode_flag & DIGITALCURLING_SIMULATE_MODE_OUT_STONE) return;
            }
        }

        if ((mode_flag & DIGITALCURLING_SIMULATE_MODE_FULL && frames > 0 && --f <= 0) ||
            (mode_flag & DIGITALCURLING_SIMULATE_MODE_COLLISION && !sim->GetCollisions().empty())
        ) {
            return;
        }
    }
}
template <typename Simulator>
DigitalCurling_ErrorCode SimulatorStepImpl(SimulatorHandle* sim, const int frames, const float sheet_width, char** out_error)
{
    if (!sim)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorStep: simulator handle is nullptr.", out_error);
    if (frames <= 0)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorStep: step frames must be positive.", out_error);

    try {
        SimulatorSimulateLoopBody(dynamic_cast<Simulator*>(sim), DIGITALCURLING_SIMULATE_MODE_FULL, frames, sheet_width);
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "SimulatorStep", out_error);
    }
}
template <typename Simulator>
DigitalCurling_ErrorCode SimulatorSimulateImpl(SimulatorHandle* sim, const DigitalCurling_SimulateModeFlag mode_flag, const float sheet_width, char** out_error)
{
    if (!sim)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorSimulate: simulator handle is nullptr.", out_error);
    if (mode_flag & DIGITALCURLING_SIMULATE_MODE_OUT_STONE && sheet_width <= digitalcurling::Stone::kRadius * 2)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorSimulate: sheet_width must be positive when OUT_STONE mode is set.", out_error);

    try {
        SimulatorSimulateLoopBody(dynamic_cast<Simulator*>(sim), mode_flag, 0, sheet_width);
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "SimulatorSimulate", out_error);
    }
}
template <typename Simulator>
DigitalCurling_ErrorCode SimulatorSetStonesImpl(SimulatorHandle* sim, const DigitalCurling_StoneCoordinate* stones, char** out_error)
{
    if (!sim)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorSetStones: simulator handle is nullptr.", out_error);
    if (!stones)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorSetStones: stones is nullptr.", out_error);

    try {
        auto* sim_ptr = dynamic_cast<Simulator*>(sim);
        digitalcurling::simulators::ISimulator::AllStones stones_data;
        for (int i = 0; i < digitalcurling::StoneCoordinate::kStoneMax; i++) {
            auto struct_stone = stones->stones[i];
            if (struct_stone.position.x == 0.f && struct_stone.position.y == 0.f && struct_stone.angle == 0.f &&
                struct_stone.translational_velocity.x == 0.f && struct_stone.translational_velocity.y == 0.f && struct_stone.angular_velocity == 0.f)
            {
                stones_data[i] = std::nullopt;
            } else {
                stones_data[i] = digitalcurling::simulators::ISimulator::StoneState{
                    digitalcurling::Vector2(struct_stone.position.x, struct_stone.position.y),
                    struct_stone.angle,
                    digitalcurling::Vector2(struct_stone.translational_velocity.x, struct_stone.translational_velocity.y),
                    struct_stone.angular_velocity
                };
            }
        }
        sim_ptr->SetStones(stones_data);
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "SimulatorSetStones", out_error);
    }
}

template <typename Simulator>
DigitalCurling_ErrorCode SimulatorAreAllStonesStoppedImpl(SimulatorHandle* sim, bool* out_stopped, char** out_error)
{
    if (!sim)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorAreAllStonesStopped: simulator handle is nullptr.", out_error);
    if (!out_stopped)
        return ReturnError(DIGITALCURLING_ERR_BUFFER_NULLPTR, "SimulatorAreAllStonesStopped: out_stopped is nullptr.", out_error);

    try {
        *out_stopped = dynamic_cast<Simulator*>(sim)->AreAllStonesStopped();
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "SimulatorAreAllStonesStopped", out_error);
    }
}
template <typename Simulator>
DigitalCurling_ErrorCode SimulatorGetStonesImpl(SimulatorHandle* sim, DigitalCurling_StoneCoordinate* out_stones, char** out_error)
{
    if (!sim)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorGetStones: simulator handle is nullptr.", out_error);
    if (!out_stones)
        return ReturnError(DIGITALCURLING_ERR_BUFFER_NULLPTR, "SimulatorGetStones: out_stones is nullptr.", out_error);

    try {
        auto* sim_ptr = dynamic_cast<Simulator*>(sim);
        auto stones = sim_ptr->GetStones();
        for (int i = 0; i < digitalcurling::StoneCoordinate::kStoneMax; i++) {
            if (stones[i].has_value()) {
                out_stones->stones[i].position.x = stones[i]->position.x;
                out_stones->stones[i].position.y = stones[i]->position.y;
                out_stones->stones[i].angle = stones[i]->angle;
                out_stones->stones[i].translational_velocity.x = stones[i]->translational_velocity.x;
                out_stones->stones[i].translational_velocity.y = stones[i]->translational_velocity.y;
                out_stones->stones[i].angular_velocity = stones[i]->angular_velocity;
            } else {
                out_stones->stones[i].position.x = 0.f;
                out_stones->stones[i].position.y = 0.f;
                out_stones->stones[i].angle = 0.f;
                out_stones->stones[i].translational_velocity.x = 0.f;
                out_stones->stones[i].translational_velocity.y = 0.f;
                out_stones->stones[i].angular_velocity = 0.f;
            }
        }
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "SimulatorGetStones", out_error);
    }
}
template <typename Simulator>
DigitalCurling_ErrorCode SimulatorGetCollisionsImpl(SimulatorHandle* sim, char** out_collisions_json, char** out_error)
{
    if (!sim)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorGetCollisions: simulator handle is nullptr.", out_error);

    try {
        auto* sim_ptr = dynamic_cast<Simulator*>(sim);
        nlohmann::json j;
        to_json(j, sim_ptr->GetCollisions());
        return ReturnString(j.dump(), "SimulatorGetCollisions", out_collisions_json, out_error);
    } catch (const std::exception& e) {
        return ReturnException(e, "SimulatorGetCollisions", out_error);
    }
}
template <typename Simulator>
DigitalCurling_ErrorCode SimulatorGetSecondsPerFrameImpl(SimulatorHandle* sim, float* out_seconds, char** out_error)
{
    if (!sim)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorGetSecondsPerFrame: simulator handle is nullptr.", out_error);
    if (!out_seconds)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorGetSecondsPerFrame: out_seconds is nullptr.", out_error);

    try {
        *out_seconds = dynamic_cast<Simulator*>(sim)->GetSecondsPerFrame();
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "SimulatorGetSecondsPerFrame", out_error);
    }
}

template <typename Simulator, moves::Shot (Simulator::*CalcShotFunc)(Vector2 const&, float, float) const>
DigitalCurling_ErrorCode SimulatorCalculateShotImpl(SimulatorHandle* sim, const DigitalCurling_Vector2* target_position, const float target_speed, const float angular_velocity,
                                       DigitalCurling_Shot* out_shot, char** out_error)
{
    if (!sim)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorCalculateShot: simulator handle is nullptr.", out_error);
    if (!target_position)
        return ReturnError(DIGITALCURLING_ERR_INVALID_ARGUMENT, "SimulatorCalculateShot: target_position is nullptr.", out_error);
    if (!out_shot)
        return ReturnError(DIGITALCURLING_ERR_BUFFER_NULLPTR, "SimulatorCalculateShot: out_shot is nullptr.", out_error);

    try {
        auto* sim_ptr = dynamic_cast<Simulator*>(sim);
        auto shot = (sim_ptr->*CalcShotFunc)({target_position->x, target_position->y}, target_speed, angular_velocity);

        out_shot->translational_velocity = shot.translational_velocity;
        out_shot->angular_velocity = shot.angular_velocity;
        out_shot->release_angle = shot.release_angle;
        return DIGITALCURLING_OK;
    } catch (const std::exception& e) {
        return ReturnException(e, "SimulatorCalculateShot", out_error);
    }
}

} // namespace

#define DIGITALCURLING_EXPORT_SIMULATOR_PLUGIN_INNER(FactoryClass, StorageClass, SimulatorClass, ...) \
    static_assert(std::is_base_of_v<digitalcurling::simulators::ISimulatorFactory, FactoryClass>, "FactoryClass must derive from ISimulatorFactory"); \
    static_assert(std::is_base_of_v<digitalcurling::simulators::ISimulatorStorage, StorageClass>, "StorageClass must derive from ISimulatorStorage"); \
    static_assert(std::is_base_of_v<digitalcurling::simulators::ISimulator, SimulatorClass>, "SimulatorClass must derive from ISimulator"); \
    \
    static const digitalcurling::plugins::SimulatorApi g_simulator_api_instance = { \
        /*get_factory*/ &digitalcurling::plugins::detail::GetFactoryImpl<digitalcurling::plugins::PluginType::simulator, FactoryClass>, \
        /*save*/ &digitalcurling::plugins::detail::TargetSaveImpl<digitalcurling::plugins::PluginType::simulator, StorageClass>, \
        /*load*/ &digitalcurling::plugins::detail::TargetLoadImpl<digitalcurling::plugins::PluginType::simulator, StorageClass>, \
        \
        /*step*/ &digitalcurling::plugins::detail::SimulatorStepImpl<SimulatorClass>, \
        /*simulate*/ &digitalcurling::plugins::detail::SimulatorSimulateImpl<SimulatorClass>, \
        /*set_stones*/ &digitalcurling::plugins::detail::SimulatorSetStonesImpl<SimulatorClass>, \
        \
        /*are_all_stones_stopped*/ &digitalcurling::plugins::detail::SimulatorAreAllStonesStoppedImpl<SimulatorClass>, \
        /*get_stones*/ &digitalcurling::plugins::detail::SimulatorGetStonesImpl<SimulatorClass>, \
        /*get_collisions*/ &digitalcurling::plugins::detail::SimulatorGetCollisionsImpl<SimulatorClass>, \
        /*get_seconds_per_frame*/ &digitalcurling::plugins::detail::SimulatorGetSecondsPerFrameImpl<SimulatorClass>, \
        \
        /*calculate_shot*/ __VA_ARGS__ \
    }; \
    DIGITALCURLING_EXPORT_PLUGIN_INNER(digitalcurling::plugins::PluginType::simulator, FactoryClass, StorageClass, SimulatorClass, nullptr, &g_simulator_api_instance)

/// @endcond


/// @brief シミュレータープラグインのエクスポート用マクロ
/// @param FactoryClass ISimulatorFactoryを継承したクラス
/// @param StorageClass ISimulatorStorageを継承したクラス
/// @param SimulatorClass ISimulatorを継承したクラス
#define DIGITALCURLING_EXPORT_SIMULATOR_PLUGIN(FactoryClass, StorageClass, SimulatorClass) \
    DIGITALCURLING_EXPORT_SIMULATOR_PLUGIN_INNER(FactoryClass, StorageClass, SimulatorClass, nullptr)

/// @brief シミュレータープラグインのエクスポート用マクロ
/// @param FactoryClass ISimulatorFactoryを継承したクラス
/// @param StorageClass ISimulatorStorageを継承したクラス
/// @param SimulatorClass ISimulatorを継承したクラス
/// @param CalculateShotFunction シミュレーターのショット逆算関数 (逆算に対応していない場合は `nullptr` )
#define DIGITALCURLING_EXPORT_INVERTIBLE_SIMULATOR_PLUGIN(FactoryClass, StorageClass, SimulatorClass, CalculateShotFunction) \
    DIGITALCURLING_EXPORT_SIMULATOR_PLUGIN_INNER(FactoryClass, StorageClass, SimulatorClass, &digitalcurling::plugins::detail::SimulatorCalculateShotImpl<SimulatorClass, CalculateShotFunction>)
