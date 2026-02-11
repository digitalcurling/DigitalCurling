// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "simulator_fcv1.hpp"

namespace digitalcurling::simulators {

SimulatorFCV1::SimulatorFCV1(SimulatorFCV1Factory const& factory)
    : SimulatorFCV1(SimulatorFCV1Storage(factory))
{}

SimulatorFCV1::SimulatorFCV1(SimulatorFCV1Storage const& storage)
    : storage_(storage)
    , world_(b2Vec2_zero)
    , stone_bodies_()
    , stones_dirty_()             // UpdateWithStorage で上書きされる
    , all_stones_stopped_()       // UpdateWithStorage でdirtyフラグがtrueになるため，後に上書きされる
    , all_stones_stopped_dirty_() // UpdateWithStorage で上書きされる
    , contact_listener_(this)
{
    b2BodyDef stone_body_def;
    stone_body_def.type = b2_dynamicBody;
    stone_body_def.awake = false;
    stone_body_def.bullet = true;
    stone_body_def.enabled = false;

    b2CircleShape stone_shape;
    stone_shape.m_radius = Stone::kRadius;

    b2FixtureDef stone_fixture_def;
    stone_fixture_def.shape = &stone_shape;
    stone_fixture_def.friction = 0.2f;  // 適当というかデフォルト値
    stone_fixture_def.restitution = 1.0; // 完全弾性衝突(完全弾性衝突の根拠は無いし多分違う)
    stone_fixture_def.restitutionThreshold = 0.f;  // 反発閾値。この値より大きい速度(m/s)で衝突すると反発が適用される。
    stone_fixture_def.density = kStoneMass / (b2_pi * Stone::kRadius * Stone::kRadius);  // kg/m^2

    for (size_t i = 0; i < StoneCoordinate::kStoneMax; ++i) {
        stone_body_def.userData.pointer = static_cast<uintptr_t>(i);
        stone_bodies_[i] = world_.CreateBody(&stone_body_def);
        stone_bodies_[i]->CreateFixture(&stone_fixture_def);
    }

    world_.SetContactListener(&contact_listener_);

    UpdateWithStorage();
}

void SimulatorFCV1::SetStones(ISimulator::AllStones const& stones)
{
    // update bodies
    for (int i = 0; i < StoneCoordinate::kStoneMax; ++i) {
        if (stones[i]) {
            auto & stone = *stones[i];
            stone_bodies_[i]->SetEnabled(true);
            stone_bodies_[i]->SetAwake(true);
            stone_bodies_[i]->SetTransform(ToB2Vec2(stone.position), stone.angle);
            stone_bodies_[i]->SetLinearVelocity(ToB2Vec2(stone.translational_velocity));
            stone_bodies_[i]->SetAngularVelocity(stone.angular_velocity);
        } else {
            stone_bodies_[i]->SetEnabled(false);
        }
    }

    stones_dirty_ = true;
    all_stones_stopped_dirty_ = true;
}

void SimulatorFCV1::Step()
{
    // simulate
    for (auto stone_body : stone_bodies_) {
        b2Vec2 normalized_stone_velocity = stone_body->GetLinearVelocity();
        float stone_speed = normalized_stone_velocity.Normalize();
        float const angular_velocity = stone_body->GetAngularVelocity();

        // 速度を計算
        // ストーンが停止してる場合は無視
        constexpr float kGravity = 9.80665f;
        if (stone_speed > std::numeric_limits<float>::epsilon()) {
            float const longitudinal_acceleration = -(0.00200985f / (stone_speed + 0.06385782f) + 0.00626286f) * kGravity;
            float const new_stone_speed = stone_speed + longitudinal_acceleration * storage_.factory.seconds_per_frame;
            if (new_stone_speed <= 0.f) {
                stone_body->SetLinearVelocity(b2Vec2_zero);
            } else {
                auto yaw_rate = [&]() -> float {
                    if (std::abs(angular_velocity) <= std::numeric_limits<float>::epsilon()) return 0.f;
                    return (angular_velocity > 0.f ? 1.0f : -1.0f) * 0.00820f * std::pow(stone_speed, -0.8f);
                };

                float const yaw = yaw_rate() * storage_.factory.seconds_per_frame;
                float const longitudinal_velocity = new_stone_speed * std::cos(yaw);
                float const transverse_velocity = new_stone_speed * std::sin(yaw);
                b2Vec2 const& e_longitudinal = normalized_stone_velocity;
                b2Vec2 const e_transverse = e_longitudinal.Skew();
                b2Vec2 const new_stone_velocity = longitudinal_velocity * e_longitudinal + transverse_velocity * e_transverse;
                stone_body->SetLinearVelocity(new_stone_velocity);
            }
        }

        // 角速度を計算
        if (std::abs(angular_velocity) > std::numeric_limits<float>::epsilon()) {
            float const angular_accel = -0.025f / std::max(stone_speed, 0.001f) * storage_.factory.seconds_per_frame;
            float new_angular_velocity = 0.f;
            if (std::abs(angular_velocity) <= std::abs(angular_accel)) {
                new_angular_velocity = 0.f;
            } else {
                new_angular_velocity = angular_velocity + angular_accel * angular_velocity / std::abs(angular_velocity);
            }
            stone_body->SetAngularVelocity(new_angular_velocity);
        }
    }

    storage_.collisions.clear();

    world_.Step(
        storage_.factory.seconds_per_frame,
        8,  // velocityIterations (公式マニュアルでの推奨値は 8)
        3); // positionIterations (公式マニュアルでの推奨値は 3)

    stones_dirty_ = true;
    all_stones_stopped_dirty_ = true;
}

ISimulator::AllStones const& SimulatorFCV1::GetStones() const
{
    if (stones_dirty_) {
        // update stones_
        for (int i = 0; i < StoneCoordinate::kStoneMax; ++i) {
            if (stone_bodies_[i]->IsEnabled()) {
                ISimulator::StoneState stone;
                stone.position = ToDigitalCurlingVector2(stone_bodies_[i]->GetWorldCenter());
                stone.angle = stone_bodies_[i]->GetAngle();
                stone.translational_velocity = ToDigitalCurlingVector2(stone_bodies_[i]->GetLinearVelocity());
                stone.angular_velocity = stone_bodies_[i]->GetAngularVelocity();
                storage_.stones[i] = stone;
            } else {
                storage_.stones[i] = std::nullopt;
            }
        }
        stones_dirty_ = false;
    }
    return storage_.stones;
}

std::vector<ISimulator::Collision> const& SimulatorFCV1::GetCollisions() const
{
    return storage_.collisions;
}

bool SimulatorFCV1::AreAllStonesStopped() const
{
    if (all_stones_stopped_dirty_) {
        all_stones_stopped_ = true;
        for (auto stone_body : stone_bodies_) {
            if (!stone_body->IsEnabled()) continue;
            if (stone_body->GetLinearVelocity().LengthSquared() > std::numeric_limits<float>::epsilon()
                || stone_body->GetAngularVelocity() > std::numeric_limits<float>::epsilon()) {
                all_stones_stopped_ = false;
                break;
            }
        }
        all_stones_stopped_dirty_ = false;
    }
    return all_stones_stopped_;
}

float SimulatorFCV1::GetSecondsPerFrame() const
{
    return storage_.factory.seconds_per_frame;
}

ISimulatorFactory const& SimulatorFCV1::GetFactory() const
{
    return storage_.factory;
}

std::unique_ptr<ISimulatorStorage> SimulatorFCV1::CreateStorage() const
{
    GetStones(); // Box2D側のデータを AllStones に適用する
    return std::make_unique<SimulatorFCV1Storage>(storage_);
}

void SimulatorFCV1::Save(ISimulatorStorage & storage) const
{
    GetStones(); // Box2D側のデータを AllStones に適用する
    static_cast<SimulatorFCV1Storage &>(storage) = storage_;
}

void SimulatorFCV1::Load(ISimulatorStorage const& storage)
{
    storage_ = static_cast<SimulatorFCV1Storage const&>(storage);
    UpdateWithStorage();
}

moves::Shot SimulatorFCV1::CalculateShot(Vector2 const& target_position, float const target_speed, float const shot_angular_velocity) const {
    if (target_speed < 0.f)
        throw std::invalid_argument("SimulatorFCV1::CalculateShot: target_speed must be non-negative.");
    if (target_speed > 4.f)
        throw std::invalid_argument("SimulatorFCV1::CalculateShot: target_speed is too large.");

    auto const target_r = target_position.Length();
    if (target_r < 0.1f)
        throw std::invalid_argument("SimulatorFCV1::CalculateShot: target_position is too close to the origin.");

    float const v0_speed = [target_r, target_speed] {
        if (target_speed <= 0.05f) {
            float constexpr kC0[] = { 0.0005048122574925176,0.2756242531609261 };
            float constexpr kC1[] = { 0.00046669575066030805,-29.898958358378636,-0.0014030973174948508 };
            float constexpr kC2[] = { 0.13968687866736632,0.41120940058777616 };

            float const c0 = kC0[0] * target_r + kC0[1];
            float const c1 = -kC1[0] * std::log(target_r + kC1[1]) + kC1[2];
            float const c2 = kC2[0] * target_r + kC2[1];

            return std::sqrt(c0 * target_speed * target_speed + c1 * target_speed + c2);
        } else if (target_speed <= 1.f) {
            float constexpr kC0[] = { -0.0014309170115803444,0.9858457898438147 };
            float constexpr kC1[] = { -0.0008339331735471273,-29.86751291726946,-0.19811799977982522 };
            float constexpr kC2[] = { 0.13967323742978,0.42816312110477517 };

            float const c0 = kC0[0] * target_r + kC0[1];
            float const c1 = -kC1[0] * std::log(target_r + kC1[1]) + kC1[2];
            float const c2 = kC2[0] * target_r + kC2[1];

            return std::sqrt(c0 * target_speed * target_speed + c1 * target_speed + c2);
        } else {
            float constexpr kC0[] = { 1.0833113118071224e-06,-0.00012132851917870833,0.004578093297561233,0.9767006869364527 };
            float constexpr kC1[] = { 0.07950648211492622,-8.228225657195706,-0.05601306077702578 };
            float constexpr kC2[] = { 0.14140440186382008,0.3875782508767419 };

            float const c0 = kC0[0] * target_r * target_r * target_r + kC0[1] * target_r * target_r + kC0[2] * target_r + kC0[3];
            float const c1 = -kC1[0] * std::log(target_r + kC1[1]) + kC1[2];
            float const c2 = kC2[0] * target_r + kC2[1];

            return std::sqrt(c0 * target_speed * target_speed + c1 * target_speed + c2);
        }
    }();

    if (target_speed >= v0_speed)
        throw std::invalid_argument("SimulatorFCV1::CalculateShot: target_speed is too large for the target_position.");

    float angular_velocity = acos(-1.0) / 2.0 * (shot_angular_velocity > 0 ? 1 : -1);
    Vector2 const delta = [angular_velocity, v0_speed, target_speed] {
        thread_local std::unique_ptr<ISimulator> s_simulator;
        if (!s_simulator)
            s_simulator = simulators::SimulatorFCV1Factory().CreateSimulator();

        ISimulator::AllStones init_stones;
        init_stones[0].emplace(Vector2(), 0.f, Vector2(0.f, v0_speed), angular_velocity);
        s_simulator->SetStones(init_stones);

        while (!s_simulator->AreAllStonesStopped()) {
            auto const& stones = s_simulator->GetStones();
            auto const speed = stones[0]->translational_velocity.Length();
            if (speed <= target_speed) return stones[0]->position;
            s_simulator->Step();
        }

        return s_simulator->GetStones()[0]->position;
    }();

    float const delta_angle = std::atan2(delta.x, delta.y); // 注: delta.x, delta.y の順番で良い
    float const target_angle = std::atan2(target_position.y, target_position.x);
    float const v0_angle = target_angle + delta_angle; // 発射方向
    return moves::Shot { v0_speed, angular_velocity, v0_angle };
}

void SimulatorFCV1::ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    auto a_body = contact->GetFixtureA()->GetBody();
    auto b_body = contact->GetFixtureB()->GetBody();

    Collision collision;
    collision.a.id = static_cast<std::uint8_t>(a_body->GetUserData().pointer);
    collision.b.id = static_cast<std::uint8_t>(b_body->GetUserData().pointer);
    collision.a.stone.position = ToDigitalCurlingVector2(a_body->GetWorldCenter());
    collision.b.stone.position = ToDigitalCurlingVector2(b_body->GetWorldCenter());
    collision.a.stone.angle = a_body->GetAngle();
    collision.b.stone.angle = b_body->GetAngle();
    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);


    collision.normal_impulse = impulse->normalImpulses[0];
    collision.tangent_impulse = impulse->tangentImpulses[0];

    instance_->storage_.collisions.emplace_back(std::move(collision));
}

void SimulatorFCV1::UpdateWithStorage()
{
    SetStones(storage_.stones);
    stones_dirty_ = false;  // storage_.stones と Box2D側のデータはすでに同期している．
    // all_stones_dirty_ = true は SetStones() 内ですでに設定されている
}

} // namespace simulators
