// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#pragma once

#include "digitalcurling/detail/i_simulator.hpp"
#include "digitalcurling/detail/simulators/simulator_fcv1_factory.hpp"
#include "simulators_simulator_fcv1_storage.hpp"
#include "box2d_util.hpp"

namespace digitalcurling::simulators {

class SimulatorFCV1Storage;

/// <summary>
/// Friction-CurlVelocity式シミュレータ1
/// </summary>
class SimulatorFCV1 : public ISimulator {
public:

    static constexpr float kStoneMass = 19.96f;  // ストーンの質量[kg]

    explicit SimulatorFCV1(SimulatorFCV1Factory const& factory);
    explicit SimulatorFCV1(SimulatorFCV1Storage const& storage);

    virtual void SetStones(AllStones const& stones) override;
    virtual void Step() override;

    virtual AllStones const& GetStones() const override;
    virtual std::vector<Collision> const& GetCollisions() const override;
    virtual bool AreAllStonesStopped() const override;
    virtual float GetSecondsPerFrame() const override;

    virtual std::string GetSimulatorId() const override
    {
        return std::string(kSimulatorFCV1Id);
    }

    virtual ISimulatorFactory const& GetFactory() const override;

    virtual std::unique_ptr<ISimulatorStorage> CreateStorage() const override;
    virtual void Save(ISimulatorStorage & storage) const override;
    virtual void Load(ISimulatorStorage const& storage) override;

    virtual ~SimulatorFCV1() = default;

    class ContactListener : public b2ContactListener {
    public:
        ContactListener(SimulatorFCV1 * instance) : instance_(instance) {}
        virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
    private:
        SimulatorFCV1 * const instance_;
    };

private:
    mutable SimulatorFCV1Storage storage_;
    b2World world_;
    std::array<b2Body*, kStoneMax> stone_bodies_;
    mutable bool stones_dirty_;
    mutable bool all_stones_stopped_;
    mutable bool all_stones_stopped_dirty_;
    ContactListener contact_listener_;

    // ストレージのデータを内部データに適用する
    void UpdateWithStorage();
};

} // namespace digitalcurling::simulation
