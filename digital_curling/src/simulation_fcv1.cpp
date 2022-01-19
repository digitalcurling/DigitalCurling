#include "simulation_fcv1.hpp"

#include <cmath>
#include <limits>

namespace digital_curling::simulation {

//----------------------------------
// SimulatorFCV1Setting

std::unique_ptr<ISimulator> SimulatorFCV1Setting::CreateSimulator() const
{
    return std::make_unique<SimulatorFCV1>(*this);
}

void SimulatorFCV1Setting::ToJson(nlohmann::json & j) const
{
    j = *this;
}

void to_json(nlohmann::json & j, const SimulatorFCV1Setting & setting)
{
    j["type"] = SimulatorFCV1Setting::kType;
    j["seconds_per_frame"] = setting.seconds_per_frame;
}

void from_json(nlohmann::json const& j, SimulatorFCV1Setting & setting)
{
    j.at("seconds_per_frame").get_to(setting.seconds_per_frame);
}


//----------------------------------
// SimulatorFCV1

namespace {

// 返り値1つめ: 正規化されたベクトル
// 返り値2つめ: もとのベクトルの長さ
inline std::pair<b2Vec2, float> Normalize(b2Vec2 const& v)
{
    b2Vec2 normalized = v;
    float length = normalized.Normalize();
    return { normalized, length };
}



// 前進加速度 [m/s^2]
inline float LongitudinalAcceleration(float speed)
{
    constexpr float kGravity = 9.80665f;
    // 鹿野ら, 氷上を進むストーンの運動の解析
    return -(0.00200985f / (speed + 0.06385782f) + 0.00626286f) * kGravity;
}



// ヨーレート 単位: [rad/s]
inline float YawRate(float speed, float angularVelocity)
{
    if (std::abs(angularVelocity) <= std::numeric_limits<float>::epsilon()) {
        return 0.f;
    }
    return (angularVelocity > 0.f ? 1.0 : -1.0) * 0.00820f * std::pow(speed, -0.8f);
}



inline float AngularAcceleration(float linearSpeed)
{
    float clampedSpeed = std::max(linearSpeed, 0.001f);
    return -0.025f / clampedSpeed;
}

} // unnamed namespace



SimulatorFCV1::SimulatorFCV1(SimulatorFCV1Setting const& setting)
    : setting_(setting)
    , world_(b2Vec2_zero)
    , stone_bodies_()
    , stones_()
    , stones_dirty_(false)
    , all_stones_stopped_(true)
    , all_stones_stopped_dirty_(false)
    , stone_collisions_()
    , contact_listener_(this)
{
    b2BodyDef stone_body_def;
    stone_body_def.type = b2_dynamicBody;
    stone_body_def.awake = false;
    stone_body_def.bullet = true;
    stone_body_def.enabled = false;

    b2CircleShape stone_shape;
    stone_shape.m_radius = kStoneRadius;

    b2FixtureDef stone_fixture_def;
    stone_fixture_def.shape = &stone_shape;
    stone_fixture_def.friction = 0.2f;  // 適当というかデフォルト値
    stone_fixture_def.restitution = 1.0; // 完全弾性衝突(完全弾性衝突の根拠は無いし多分違う)
    stone_fixture_def.restitutionThreshold = 0.f;  // 反発閾値。この値より大きい速度(m/s)で衝突すると反発が適用される。
    stone_fixture_def.density = kStoneMass / (b2_pi * kStoneRadius * kStoneRadius);  // kg/m^2

    for (size_t i = 0; i < kStoneMax; ++i) {
        stone_body_def.userData.pointer = static_cast<uintptr_t>(i);
        stone_bodies_[i] = world_.CreateBody(&stone_body_def);
        stone_bodies_[i]->CreateFixture(&stone_fixture_def);
    }

    for (auto & stone : stones_) {
        stone = std::nullopt;
    }

    world_.SetContactListener(&contact_listener_);
}



void SimulatorFCV1::SetStones(AllStoneData const& stones)
{
    // update bodies
    for (size_t i = 0; i < kStoneMax; ++i) {
        if (stones[i]) {
            auto & stone = *stones[i];
            stone_bodies_[i]->SetEnabled(true);
            stone_bodies_[i]->SetAwake(true);
            stone_bodies_[i]->SetTransform(ToB2Vec2(stone.position), stone.angle);
            stone_bodies_[i]->SetLinearVelocity(ToB2Vec2(stone.linear_velocity));
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
        b2Vec2 const stone_velocity = stone_body->GetLinearVelocity();  // copy
        auto const [normalized_stone_velocity, stone_speed] = Normalize(stone_velocity);
        float const angular_velocity = stone_body->GetAngularVelocity();

        // 速度を計算
        // ストーンが停止してる場合は無視
        if (stone_speed > std::numeric_limits<float>::epsilon()) {
            // ストーンの速度を計算
            float const new_stone_speed = stone_speed + LongitudinalAcceleration(stone_speed) * setting_.seconds_per_frame;
            if (new_stone_speed <= 0.f) {
                stone_body->SetLinearVelocity(b2Vec2_zero);
            } else {
                float const yaw = YawRate(stone_speed, angular_velocity) * setting_.seconds_per_frame;
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
            float const angular_accel = AngularAcceleration(stone_speed) * setting_.seconds_per_frame;
            float new_angular_velocity = 0.f;
            if (std::abs(angular_velocity) <= std::abs(angular_accel)) {
                new_angular_velocity = 0.f;
            } else {
                new_angular_velocity = angular_velocity + angular_accel * angular_velocity / std::abs(angular_velocity);
            }
            stone_body->SetAngularVelocity(new_angular_velocity);
        }
    }

    stone_collisions_.clear();

    world_.Step(
        setting_.seconds_per_frame,
        8,  // velocityIterations (公式マニュアルでの推奨値は 8)
        3); // positionIterations (公式マニュアルでの推奨値は 3)

    stones_dirty_ = true;
    all_stones_stopped_dirty_ = true;
}



AllStoneData const& SimulatorFCV1::GetStones() const
{
    if (stones_dirty_) {
        // update stones_
        for (size_t i = 0; i < kStoneMax; ++i) {
            if (stone_bodies_[i]->IsEnabled()) {
                StoneData stone;
                stone.position = ToDC2Vector2(stone_bodies_[i]->GetWorldCenter());
                stone.angle = stone_bodies_[i]->GetAngle();
                stone.linear_velocity = ToDC2Vector2(stone_bodies_[i]->GetLinearVelocity());
                stone.angular_velocity = stone_bodies_[i]->GetAngularVelocity();
                stones_[i] = stone;
            } else {
                stones_[i] = std::nullopt;
            }
        }
        stones_dirty_ = false;
    }
    return stones_;
}



std::vector<StoneCollision> const& SimulatorFCV1::GetCollisions() const
{
    return stone_collisions_;
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
    return setting_.seconds_per_frame;
}



ISimulatorSetting const& SimulatorFCV1::GetSetting() const
{
    return setting_;
}



void SimulatorFCV1::ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    auto a_body = contact->GetFixtureA()->GetBody();
    auto b_body = contact->GetFixtureB()->GetBody();

    StoneCollision collision;
    collision.a_id = static_cast<std::uint8_t>(a_body->GetUserData().pointer);
    collision.b_id = static_cast<std::uint8_t>(b_body->GetUserData().pointer);
    collision.a_position = ToDC2Vector2(a_body->GetWorldCenter());
    collision.b_position = ToDC2Vector2(b_body->GetWorldCenter());
    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);


    collision.normal_impulse = impulse->normalImpulses[0];
    collision.tangent_impulse = impulse->tangentImpulses[0];

    instance_->stone_collisions_.emplace_back(std::move(collision));
}

} // namespace digital_curling::simulation
