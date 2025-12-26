// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief ISimulator を定義

#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include "digitalcurling/stone.hpp"
#include "digitalcurling/stone_coordinate.hpp"
#include "digitalcurling/vector2.hpp"

namespace digitalcurling::simulators {

class ISimulatorFactory;
class ISimulatorStorage;

/// @brief ストーンの挙動を再現する物理シミュレータを扱うためのインターフェース
///
/// `ISimulator` のインスタンスは `ISimulatorFactory` の派生クラスの
/// `ISimulatorFactory::CreateInstance()` でのみ生成されます。
///
/// `ISimulatorStorage` を用いてシミュレーション中の状態を保存/復元することができます。
/// 状態の保存には `ISimulator::CreateStorage()` または `ISimulator::Save()` を、
/// 状態の復元には `ISimulatorStorage::CreateSimulator()` または `ISimulator::Load()` を使用してください。
///
/// @sa ISimulatorFactory
class ISimulator {
public:

    // types ---

    /// @brief ストーンの位置，角度，速度，角速度を格納します
    struct StoneState : public Stone {
        /// @brief 速度(m/s)
        Vector2 translational_velocity;
        /// @brief 角速度(rad/s)
        float angular_velocity;

        /// @brief 全データを0で初期化します
        StoneState()
            : Stone()
            , translational_velocity()
            , angular_velocity(0.f) {}

        /// @brief 与えられたデータで初期化
        /// @param[in] position 位置(m)
        /// @param[in] angle 角度(radian)
        /// @param[in] translational_velocity 速度(m/s)
        /// @param[in] angular_velocity 角速度(radian/s)
        StoneState(Vector2 position, float angle, Vector2 translational_velocity, float angular_velocity)
            : Stone(position, angle)
            , translational_velocity(translational_velocity)
            , angular_velocity(angular_velocity) {}
    };

    /// @brief 全ストーンの位置と速度
    ///
    /// 盤面に存在しないストーンは `std::nullopt` として表されます。
    using AllStones = std::array<std::optional<StoneState>, StoneCoordinate::kStoneMax>;

    /// @brief ストーンどうしの衝突の情報
    struct Collision {
        /// @brief 衝突に関するストーンの情報
        struct CollisionStone {
            std::uint8_t id; ///< ストーンのID
            StoneState stone; ///< ストーンの位置と角度

            /// @brief デフォルトコンストラクタ
            CollisionStone() : id(0), stone() {}

            /// @brief 与えられたデータで初期化する
            /// @param[in] id ストーンのID
            /// @param[in] stone ストーンの位置と角度
            CollisionStone(std::uint8_t id, StoneState const& stone) : id(id), stone(stone) {}
        };
        CollisionStone a; ///< 衝突したストーン
        CollisionStone b; ///< 衝突したストーン
        float normal_impulse; ///< 法線方向の撃力
        float tangent_impulse; ///< 接線方向の撃力

        /// @brief 全パラメータを 0 で初期化
        Collision()
            : a()
            , b()
            , normal_impulse(0.f)
            , tangent_impulse(0.f) {}

        /// @brief 与えられたパラメータで初期化する
        /// @param[in] a ストーンAのIDと位置
        /// @param[in] b ストーンBのIDと位置
        /// @param[in] normal_impulse 法線方向の撃力
        /// @param[in] tangent_impulse 接線方向の撃力
        Collision(CollisionStone a, CollisionStone b, float normal_impulse, float tangent_impulse)
            : a(a)
            , b(b)
            , normal_impulse(normal_impulse)
            , tangent_impulse(tangent_impulse) {}

        /// @brief ストーンどうしが接した座標を得る
        ///
        /// @returns ストーンどうしが接した座標
        Vector2 GetContactPoint() const
        {
            return (a.stone.position + b.stone.position) * 0.5f;
        }
    };

// member functions ---
protected:
    /// @brief デフォルトコンストラクタ
    ISimulator() = default;
    /// @brief コピーコンストラクタ
    ISimulator(ISimulator const&) = default;
    /// @brief コピー代入演算子
    ISimulator& operator = (ISimulator const&) = default;

public:
    virtual ~ISimulator() = default;

    /// @brief 全ストーンの情報を設定する
    /// @param[in] stones 全ストーンの情報
    virtual void SetStones(AllStones const& stones) = 0;

    /// @brief 1フレーム進める
    ///
    /// 1回呼び出すと `GetSecondsPerFrame()` の値だけシミュレータ内部の時間が進みます
    virtual void Step() = 0;

    /// @brief 全ストーンの情報を取得する
    ///
    /// @note
    /// このメンバ関数の返り値として得られた参照は他のconstでないメンバ関数を呼び出した時点で無効になります
    /// 無効になった参照へアクセスした場合の動作は規定されません。
    /// 他のconstでないメンバ関数を呼び出した後も返り値の値を保持したい場合はコピーしてください。
    ///
    /// @returns 全ストーンの情報
    virtual AllStones const& GetStones() const = 0;

    /// @brief 直前の `Step()` の呼出しで発生したすべての衝突の情報を得る
    ///
    /// @note
    /// このメンバ関数の返り値として得られた参照は他のconstでないメンバ関数を呼び出した時点で無効になります
    /// 無効になった参照へアクセスした場合の動作は規定されません。
    /// 他のconstでないメンバ関数を呼び出した後も返り値の値を保持したい場合はコピーしてください。
    ///
    /// @returns 直前の `Step()` の呼出しで発生したすべての衝突
    virtual std::vector<Collision> const& GetCollisions() const = 0;

    /// @brief 全ストーンが停止しているかをチェックする
    /// @returns 全ストーンが停止していれば `true`
    virtual bool AreAllStonesStopped() const = 0;

    /// @brief 1回の `Step()` 呼び出しで進む時間を得る
    /// @returns 1回の `Step()` 呼び出しで進む時間(秒)
    virtual float GetSecondsPerFrame() const = 0;

    /// @brief シミュレータIDを得る
    ///
    /// シミュレータIDはシミュレータの種類ごとに異なります。
    ///
    /// @returns シミュレータID
    virtual std::string GetSimulatorId() const = 0;

    /// @brief ファクトリーを得る
    ///
    /// 得られたファクトリーはこの `ISimulator` インスタンスを生成した `ISimulatorFactory` インスタンスよりも
    /// 詳細な情報を持っている可能性があります。(例: インスタンスを生成した `ISimulatorFactory` の
    /// シード値が未指定であっても、 `GetFactory()` の返り値ではシード値が補完されているなど)
    /// 得られた `ISimulatorFactory` から `ISimulator` を生成することで、同じ初期条件のシミュレータを得ることができます。
    ///
    /// @note
    /// このメンバ関数の返り値として得られた参照は他のconstでないメンバ関数を呼び出した時点で無効になります。
    /// 無効になった参照へアクセスした場合の動作は規定されません。
    /// 他のconstでないメンバ関数を呼び出した後も返り値の値を保持したい場合はコピーしてください。
    ///
    /// @returns ファクトリー
    virtual ISimulatorFactory const& GetFactory() const = 0;

    /// @brief ストレージを生成して現在の状態を保存する
    /// @returns 新たに生成されたストレージ (シミュレーション状態のセーブ済み)
    virtual std::unique_ptr<ISimulatorStorage> CreateStorage() const = 0;

    /// @brief ストレージに現在の状態を保存する
    /// @param[out] storage ストレージ
    virtual void Save(ISimulatorStorage & storage) const = 0;

    /// @brief ストレージから状態を復元する
    /// @param[in] storage ストレージ
    virtual void Load(ISimulatorStorage const& storage) = 0;
};


/// @cond Doxygen_Suppress
// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ISimulator::StoneState, position, angle, translational_velocity, angular_velocity)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ISimulator::Collision::CollisionStone, id, stone)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ISimulator::Collision, a, b, normal_impulse, tangent_impulse)
/// @endcond

} // namespace digitalcurling::simulators
