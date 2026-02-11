// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PluginSimulator, InvertiblePluginSimulator を定義

#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>
#include <uuidv7/uuidv7.hpp>
#include "digitalcurling/plugins/detail/plugin_resource.hpp"
#include "digitalcurling/plugins/wrapper_base.hpp"
#include "digitalcurling/simulators/i_simulator.hpp"

namespace digitalcurling::simulators {


/// @brief プラグインを経由した物理シミュレーター
class PluginSimulator : public plugins::WrapperBase<digitalcurling::simulators::ISimulator> {
public:
    /// @brief コンストラクタ
    /// @param simulator_id シミュレーターID
    /// @param instance_id インスタンスID
    /// @param owner_resource 実際のインスタンスを持つリソースへの参照
    PluginSimulator(
        std::string simulator_id,
        uuidv7::uuidv7 instance_id,
        std::weak_ptr<OwnerResource> owner_resource
    );

    virtual void SetStones(ISimulator::AllStones const& stones) override;
    virtual void Step() override { Step(1, 0.0f); }

    virtual ISimulator::AllStones const& GetStones() const override;
    virtual std::vector<ISimulator::Collision> const& GetCollisions() const override;
    virtual bool AreAllStonesStopped() const override;
    virtual float GetSecondsPerFrame() const override;
    virtual ISimulatorFactory const& GetFactory() const override;

    virtual std::unique_ptr<ISimulatorStorage> CreateStorage() const override;
    virtual void Save(ISimulatorStorage & storage) const override;
    virtual void Load(ISimulatorStorage const& storage) override;

    /// @brief 指定されたフレーム進める
    /// @param[in] frames 進めるフレーム数
    /// @param[in] sheet_width シートの幅(m)
    virtual void Step(int frames, float sheet_width);

    /// @brief シミュレーションを実行する
    /// @param mode_flag 停止条件のフラグ
    /// @param sheet_width シートの幅(m)
    virtual void Simulate(SimulateModeFlag mode_flag, float sheet_width);

private:
    mutable std::mutex mutex_;
    mutable std::unique_ptr<ISimulatorFactory> factory_cache_;
    mutable std::optional<ISimulator::AllStones> all_stones_cache_;
    mutable std::optional<std::vector<ISimulator::Collision>> collisions_cache_;

    inline void ClearCaches() {
        std::lock_guard<std::mutex> lock(mutex_);
        factory_cache_.reset();
        all_stones_cache_.reset();
        collisions_cache_.reset();
    }
};

/// @brief プラグインを経由した逆算可能な物理シミュレーター
/// @note
/// ここでの「逆算可能」とは、目標のストーンの位置・速度を実現するショットを一定の精度で計算できることを指し、
/// シミュレーションの逆再生や完全な逆算が可能であることを意味しません。
class InvertiblePluginSimulator : public PluginSimulator {
public:
    using PluginSimulator::PluginSimulator;

    /// @brief 指定地点を指定速度で通過するショットを逆算する
    /// @param target_position 目標地点
    /// @param target_speed 目標地点到達時の速度
    /// @param angular_velocity ショットの回転速度
    /// @return 逆算されたショット
    virtual moves::Shot CalculateShot(Vector2 const& target_position, float target_speed, float angular_velocity) const;
};

} // namespace digitalcurling::simulators
