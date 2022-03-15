#ifndef DIGITAL_CURLING_SRC_SIMULATORS_FCV1_SIMULATOR_HPP
#define DIGITAL_CURLING_SRC_SIMULATORS_FCV1_SIMULATOR_HPP

#include "digital_curling/detail/i_simulator.hpp"
#include "digital_curling/detail/simulators/fcv1_simulator_factory.hpp"
#include "simulators_fcv1_simulator_storage.hpp"
#include "box2d_util.hpp"

namespace digital_curling::simulators {

class FCV1SimulatorStorage;

/// <summary>
/// Friction-CurlVelocity式シミュレータ1
/// </summary>
class FCV1Simulator : public ISimulator {
public:

    static constexpr float kStoneMass = 19.96f;  // ストーンの質量[kg]

    explicit FCV1Simulator(FCV1SimulatorFactory const& factory);
    explicit FCV1Simulator(FCV1SimulatorStorage const& storage);

    virtual void SetStones(AllStones const& stones) override;
    virtual void Step() override;

    virtual AllStones const& GetStones() const override;
    virtual std::vector<Collision> const& GetCollisions() const override;
    virtual bool AreAllStonesStopped() const override;
    virtual float GetSecondsPerFrame() const override;

    virtual std::string GetSimulatorId() const override
    {
        return std::string(kFCV1SimulatorId);
    }

    virtual ISimulatorFactory const& GetFactory() const override;

    virtual std::unique_ptr<ISimulatorStorage> CreateStorage() const override;
    virtual void Save(ISimulatorStorage & storage) const override;
    virtual void Load(ISimulatorStorage const& storage) override;

    virtual ~FCV1Simulator() = default;

    class ContactListener : public b2ContactListener {
    public:
        ContactListener(FCV1Simulator * instance) : instance_(instance) {}
        virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
    private:
        FCV1Simulator * const instance_;
    };

private:
    mutable FCV1SimulatorStorage storage_;
    b2World world_;
    std::array<b2Body*, kStoneMax> stone_bodies_;
    mutable bool stones_dirty_;
    mutable bool all_stones_stopped_;
    mutable bool all_stones_stopped_dirty_;
    ContactListener contact_listener_;

    // ストレージのデータを内部データに適用する
    void UpdateWithStorage();
};

} // namespace digital_curling::simulation

#endif // DIGITAL_CURLING_SRC_SIMULATORS_FCV1_SIMULATOR_HPP
