/// \file
/// \brief IShotRandomizer を定義します

#ifndef DIGITAL_CURLING_I_PLAYER_HPP
#define DIGITAL_CURLING_I_PLAYER_HPP

#include <string>
#include <memory>
#include "vector2.hpp"
#include "moves/shot.hpp"

namespace digital_curling {

class IPlayerFactory;
class IPlayerStorage;


/// \brief 理想的なショットを与えられた際に，乱数を加えたりしてプレイヤーによるブレを付与する
///
/// IPlayerStorage を用いて内部状態を保存/復元することができます．
/// 状態の保存には IPlayer::CreateStorage() または IPlayer::Save() を，
/// 状態の復元には IPlayerStorage::CreatePlayer() または IPlayer::Load() を使用します．
class IPlayer {
public:
    IPlayer() = default;
    IPlayer(IPlayer const&) = delete;
    IPlayer & operator = (IPlayer const&) = delete;
    virtual ~IPlayer() = default;

    /// \brief ショットを行う．
    ///
    /// \param[in] shot 理想的なショット
    /// \returns プレイヤーによってプレイされたショット
    virtual moves::Shot Play(moves::Shot const& shot) = 0;

    /// \brief プレイヤーIDを得る．
    ///
    /// プレイヤーIDはプレイヤーの種類ごとに異なる．
    ///
    /// \returns プレイヤーID
    virtual std::string GetPlayerId() const = 0;

    /// \brief ファクトリーを得る
    ///
    /// 得られたファクトリーはこの IPlayer インスタンスを生成した IPlayerFactory インスタンスよりも
    /// 詳細な情報を持っている可能性があります(例: インスタンスを生成した IPlayerFactory の
    /// シード値が未指定であっても， GetFactory() の返り値ではシード値が補完されている．など)．
    /// 得られた IPlayerFactory から IPlayer を生成することで，同じ初期条件のプレイヤーを得ることができます．
    ///
    /// \returns ファクトリー
    virtual IPlayerFactory const& GetFactory() const = 0;

    /// \brief ストレージを生成する
    ///
    /// \returns 新たに生成されたストレージ
    virtual std::unique_ptr<IPlayerStorage> CreateStorage() const = 0;

    /// \brief ストレージに状態を保存する
    ///
    /// \param[out] storage ストレージ
    virtual void Save(IPlayerStorage & storage) const = 0;

    /// \brief ストレージから状態を復元する
    ///
    /// \param[in] storage ストレージ
    virtual void Load(IPlayerStorage const& storage) = 0;
};



} // namespace digital_curling

#endif // DIGITAL_CURLING_I_PLAYER_HPP
