#ifndef DIGITAL_CURLING_SRC_SIMULATION_FCV1_HPP
#define DIGITAL_CURLING_SRC_SIMULATION_FCV1_HPP

#include "digital_curling/detail/simulation/simulator.hpp"
#include "digital_curling/detail/simulation/simulator_fcv1_setting.hpp"
#include "box2d_util.hpp"

namespace digital_curling::simulation {

/// <summary>
/// Friction-CurlVelocity式シミュレータ1
/// </summary>
class SimulatorFCV1 : public ISimulator {
public:

    static constexpr float kStoneRadius = 0.145f;  // ストーンの半径[m]
    static constexpr float kStoneMass = 19.96f;  // ストーンの質量[kg]

    explicit SimulatorFCV1(SimulatorFCV1Setting const& setting);

    virtual void SetStones(AllStoneData const& stones) override;
    virtual void Step() override;

    virtual AllStoneData const& GetStones() const override;
    virtual std::vector<StoneCollision> const& GetCollisions() const override;
    virtual bool AreAllStonesStopped() const override;
    virtual float GetSecondsPerFrame() const override;

    virtual float GetStoneRadius() const override
    {
        return kStoneRadius;
    }

    virtual ISimulatorSetting const& GetSetting() const override;

    virtual ~SimulatorFCV1() = default;

    class ContactListener : public b2ContactListener {
    public:
        ContactListener(SimulatorFCV1 * instance) : instance_(instance) {}
        virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
    private:
        SimulatorFCV1 * const instance_;
    };

private:
    SimulatorFCV1Setting const setting_;
    b2World world_;
    std::array<b2Body*, kStoneMax> stone_bodies_;
    mutable AllStoneData stones_;
    mutable bool stones_dirty_;
    mutable bool all_stones_stopped_;
    mutable bool all_stones_stopped_dirty_;
    std::vector<StoneCollision> stone_collisions_;
    ContactListener contact_listener_;
};

} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SRC_SIMULATION_FCV1_HPP
