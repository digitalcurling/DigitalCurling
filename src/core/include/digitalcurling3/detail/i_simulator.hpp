// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file
/// \brief ISimulator を定義します

#ifndef DIGITALCURLING3_I_SIMULATOR_HPP
#define DIGITALCURLING3_I_SIMULATOR_HPP

#include <cstdint>
#include <array>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include "vector2.hpp"
#include "transform.hpp"

namespace digitalcurling3 {

class ISimulatorFactory;
class ISimulatorStorage;

/// \brief ストーンの挙動を再現する物理シミュレータを扱うためのインターフェイス
///
/// ISimulator のインスタンスは ISimulatorFactory の派生クラスの
/// ISimulatorFactory::CreateInstance() でのみ生成されます．
///
/// このシミュレータではカーリングのルールやアイスシートの大きさ制限は記述されません．
/// それらについては ApplyMove() で記述されます．
///
/// ISimulatorStorage を用いてシミュレーション中の状態を保存/復元することができます．
/// この機能は木探索などで使用されることが想定されます．
/// 状態の保存には ISimulator::CreateStorage() または ISimulator::Save() を，
/// 状態の復元には ISimulatorStorage::CreateSimulator() または ISimulator::Load() を使用します．
///
/// \sa ISimulatorFactory, ApplyMove()
class ISimulator {
public:

    // constants ---

    /// \brief シミュレータ上に配置可能なストーンの最大数
    static constexpr std::uint8_t kStoneMax = 16;

    /// \brief ストーンの半径(m)
    static constexpr float kStoneRadius = 0.145f;


    // types ---

    /// \brief ストーンの位置，角度，速度，角速度を格納します
    struct Stone : public Transform {
        Vector2 linear_velocity; ///< 速度(m/s)
        float angular_velocity; ///< 角速度(radian/s)

        /// \brief 全データを0で初期化します
        Stone()
            : Transform()
            , linear_velocity()
            , angular_velocity(0.f) {}

        /// \brief 与えられたデータで初期化します
        ///
        /// \param[in] position 位置(m)
        /// \param[in] angle 角度(radian)
        /// \param[in] linear_velocity 速度(m/s)
        /// \param[in] angular_velocity 角速度(radian/s)
        Stone(Vector2 position, float angle, Vector2 linear_velocity, float angular_velocity)
            : Transform(position, angle)
            , linear_velocity(linear_velocity)
            , angular_velocity(angular_velocity) {}
    };

    /// \brief 全ストーンの位置と速度
    ///
    /// 盤面に存在しないストーンは \c std::nullopt として表されます．
    using AllStones = std::array<std::optional<Stone>, kStoneMax>;

    /// \brief ストーンどうしの衝突の情報
    struct Collision {
        /// \brief 衝突に関するストーンの情報
        struct Stone {
            std::uint8_t id; ///< ストーンのID
            Transform transform; ///< ストーンの位置と角度

            /// \brief デフォルトコンストラクタ
            Stone() : id(0), transform() {}

            /// \brief 与えられたデータで初期化します
            ///
            /// \param[in] id ストーンのID
            /// \param[in] transform ストーンの位置と角度
            Stone(std::uint8_t id, Transform const& transform) : id(id), transform(transform) {}
        };
        Stone a; ///< 衝突したストーン
        Stone b; ///< 衝突したストーン
        float normal_impulse; ///< 法線方向の撃力
        float tangent_impulse; ///< 接線方向の撃力

        /// \brief 全パラメータを 0 で初期化します
        Collision()
            : a()
            , b()
            , normal_impulse(0.f)
            , tangent_impulse(0.f) {}

        /// \brief 与えられたパラメータで初期化します
        ///
        /// \param[in] a_id ストーンAのID
        /// \param[in] b_id ストーンBのID
        /// \param[in] a_transform ストーンAの位置
        /// \param[in] b_transform ストーンBの位置
        /// \param[in] normal_impulse 法線方向の撃力
        /// \param[in] tangent_impulse 接線方向の撃力
        Collision(std::uint8_t a_id, std::uint8_t b_id, Transform const& a_transform, Transform const& b_transform, float normal_impulse, float tangent_impulse)
            : a(a_id, a_transform)
            , b(b_id, b_transform)
            , normal_impulse(normal_impulse)
            , tangent_impulse(tangent_impulse) {}

        /// \brief ストーンどうしが接した座標を得る。
        ///
        /// \returns ストーンどうしが接した座標
        Vector2 GetContactPoint() const
        {
            return (a.transform.position + b.transform.position) * 0.5f;
        }
    };


    // member functions ---

protected:
    ISimulator() = default;
    ISimulator(ISimulator const&) = default; ///< コピーコンストラクタ．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．
    ISimulator& operator = (ISimulator const&) = default; ///< コピー代入演算子．何もコピーしません．サブクラスでのdefault実装をサポートするためのものです．

public:
    virtual ~ISimulator() = default;

    /// \brief 全ストーンの情報を設定する．
    ///
    /// \param[in] stones 全ストーンの情報(位置，角度，速度，角速度)
    virtual void SetStones(AllStones const& stones) = 0;

    /// \brief 1フレーム進める．
    ///
    /// 1回呼び出すと GetSecondsPerFrame() の返り値の分だけシミュレータ内部の時間が進みます
    virtual void Step() = 0;

    /// \brief 全ストーンの情報を取得する．
    ///
    /// \note
    /// このメンバ関数の返り値として得られた参照は他のconstでないメンバ関数を呼び出した時点で無効になります．
    /// 無効になった参照へアクセスした場合の動作は規定されません．
    /// 他のconstでないメンバ関数を呼び出した後も返り値の値を保持したい場合はコピーしてください．
    ///
    /// \returns 全ストーンの情報(位置，角度，速度，角速度)
    virtual AllStones const& GetStones() const = 0;

    /// \brief 直前の Step() の呼出しで発生したすべての衝突の情報を得る．
    ///
    /// \note
    /// このメンバ関数の返り値として得られた参照は他のconstでないメンバ関数を呼び出した時点で無効になります．
    /// 無効になった参照へアクセスした場合の動作は規定されません．
    /// 他のconstでないメンバ関数を呼び出した後も返り値の値を保持したい場合はコピーしてください．
    ///
    /// \returns 直前の Step() の呼出しで発生したすべての衝突
    virtual std::vector<Collision> const& GetCollisions() const = 0;

    /// \brief 全ストーンが停止しているかをチェックする．
    ///
    /// \returns 全ストーンが停止していれば \c true ，一つでも停止していないストーンがあれば \c false
    virtual bool AreAllStonesStopped() const = 0;

    /// \brief 1回 Step() の呼出しで進む時間を得る．
    ///
    /// \returns 1回の Step() の呼出しで進む時間(秒)
    virtual float GetSecondsPerFrame() const = 0;

    /// \brief シミュレータIDを得る．
    ///
    /// シミュレータIDはシミュレータの種類ごとに異なる．
    ///
    /// \returns シミュレータID
    virtual std::string GetSimulatorId() const = 0;

    /// \brief ファクトリーを得る
    ///
    /// 得られたファクトリーはこの ISimulator インスタンスを生成した ISimulatorFactory インスタンスよりも
    /// 詳細な情報を持っている可能性があります(例: インスタンスを生成した ISimulatorFactory の
    /// シード値が未指定であっても， GetFactory() の返り値ではシード値が補完されている．など)．
    /// 得られた ISimulatorFactory から ISimulator を生成することで，同じ初期条件のシミュレータを得ることができます．
    ///
    /// \note
    /// このメンバ関数の返り値として得られた参照は他のconstでないメンバ関数を呼び出した時点で無効になります．
    /// 無効になった参照へアクセスした場合の動作は規定されません．
    /// 他のconstでないメンバ関数を呼び出した後も返り値の値を保持したい場合はコピーしてください．
    ///
    /// \returns ファクトリー
    virtual ISimulatorFactory const& GetFactory() const = 0;

    /// \brief ストレージを生成し，現在の状態を保存する
    ///
    /// \returns 新たに生成されたストレージ (シミュレーション状態のセーブ済み)
    virtual std::unique_ptr<ISimulatorStorage> CreateStorage() const = 0;

    /// \brief ストレージに現在の状態を保存する
    ///
    /// \param[out] storage ストレージ
    virtual void Save(ISimulatorStorage & storage) const = 0;

    /// \brief ストレージから状態を復元する
    ///
    /// \param[in] storage ストレージ
    virtual void Load(ISimulatorStorage const& storage) = 0;
};


// json

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ISimulator::Stone,
    position,
    angle,
    linear_velocity,
    angular_velocity
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ISimulator::Collision::Stone,
    id,
    transform
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ISimulator::Collision,
    a,
    b,
    normal_impulse,
    tangent_impulse
)

} // namespace digitalcurling3

#endif // DIGITALCURLING3_I_SIMULATOR_HPP
