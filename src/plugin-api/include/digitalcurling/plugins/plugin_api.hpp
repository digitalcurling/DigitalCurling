// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief プラグインのAPIを定義

#pragma once

#include "digitalcurling/plugins/i_plugin_object.hpp"
#include "digitalcurling/plugins/data_object.h"

/// @brief プラグインAPIのバージョン
/// @ingroup plugin_api
#define DIGITALCURLING_PLUGIN_API_VERSION 1

namespace digitalcurling::plugins {


/// @addtogroup plugin_api
/// @{

// --- Plugin Common Functions ---

/// @brief APIから取得した文字列を解放する関数ポインタ型
/// @param[in] str 解放する文字列のポインタ
typedef void (*FreeStringFunc)(char* str);

/// @brief Factory オブジェクトを生成する関数ポインタ型
/// @param[in] json 初期化パラメータ（JSON形式の文字列）
/// @param[out] out_handle 生成された Factory ハンドルを格納するポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ (呼び出し側で解放が必要)
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*CreateFactoryFunc)(const char* json, FactoryHandle** out_handle, char** out_error);

/// @brief Storage オブジェクトを生成する関数ポインタ型
/// @param[in] json 初期化パラメータ（JSON形式の文字列）
/// @param[out] out_handle 生成された Storage ハンドルを格納するポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ (呼び出し側で解放が必要)
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*CreateStorageFunc)(const char* json, StorageHandle** out_handle, char** out_error);

/// @brief Player/Simulator インスタンスを生成する関数ポインタ型
/// @param[in] creator Factory ハンドル
/// @param[out] out_target_handle 生成された Player/Simulator ハンドルを格納するポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ (呼び出し側で解放が必要)
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*CreateTargetFunc)(CreatorHandle* creator, TargetHandle** out_target_handle, char** out_error);

/// @brief オブジェクトを破棄する関数ポインタ型
/// @param[in] handle 破棄するオブジェクトのハンドル
typedef void (*DestroyObjectFunc)(void* handle);


// --- Creator Common Functions ---

/// @brief Creator (Factory/Storage) の現在の状態をJSON形式で取得する関数ポインタ型
/// @param[in] creator Creator ハンドル
/// @param[out] out_json 状態を表すJSON文字列を格納するポインタ (呼び出し側で解放が必要)
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ (呼び出し側で解放が必要)
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*ObjectCreatorGetStateFunc)(CreatorHandle* creator, char** out_json, char** out_error);

/// @brief Factory の状態を設定・更新する関数ポインタ型
/// @param[in] creator Factory ハンドル
/// @param[in] json 更新内容を含むJSON文字列
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ (呼び出し側で解放が必要)
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*FactorySetStateFunc)(FactoryHandle* creator, const char* json, char** out_error);

/// @brief Storage の状態を設定・更新する関数ポインタ型
/// @param[in] creator Storage ハンドル
/// @param[in] json 更新内容を含むJSON文字列
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ (呼び出し側で解放が必要)
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*StorageSetStateFunc)(StorageHandle* creator, const char* json, char** out_error);


// --- Player Plugin Functions Definition ---

/// @brief Player インスタンスから、そのインスタンスを生成した Factory を取得する関数ポインタ型
/// @param[in] player Player ハンドル
/// @param[out] out_factory_handle 生成された Factory ハンドルを格納するポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*PlayerGetFactoryFunc)(PlayerHandle* player, FactoryHandle** out_factory_handle, char** out_error);

/// @brief Player の現在の状態を Storage に保存する関数ポインタ型
/// @param[in] player Player ハンドル
/// @param[in] storage_handle 保存先の Storage ハンドル
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*PlayerSaveFunc)(PlayerHandle* player, StorageHandle* storage_handle, char** out_error);

/// @brief Storage から Player の状態を読み込む関数ポインタ型
/// @param[in] player Player ハンドル
/// @param[in] storage_handle 読み込み元の Storage ハンドル
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*PlayerLoadFunc)(PlayerHandle* player, StorageHandle* storage_handle, char** out_error);

/// @brief Player の性別を取得する関数ポインタ型
/// @param[in] player Player ハンドル
/// @param[out] out_gender 取得した性別を格納するポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*PlayerGetGenderFunc)(PlayerHandle* player, DigitalCurling_Gender* out_gender, char** out_error);

/// @brief Player によって実際にプレイされたショットを取得する関数ポインタ型
/// @param[in] player Player ハンドル
/// @param[in] shot 理想的なショット情報へのポインタ
/// @param[out] out_shot Player によってプレイされたショット情報を格納するポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*PlayerPlayFunc)(PlayerHandle* player, const DigitalCurling_Shot* shot, DigitalCurling_Shot* out_shot, char** out_error);

/// @brief プレイヤープラグイン固有のAPI関数テーブル
struct PlayerApi {
    /// @brief PlayerインスタンスからFactoryを取得する関数
    PlayerGetFactoryFunc get_factory;
    /// @brief Playerの状態をStorageに保存する関数
    PlayerSaveFunc save;
    /// @brief StorageからPlayerの状態を読み込む関数
    PlayerLoadFunc load;

    /// @brief Playerの性別を取得する関数
    PlayerGetGenderFunc get_gender;
    /// @brief Playerによってプレイされたショットを取得する関数
    PlayerPlayFunc play;
};


// --- Simulator Plugin Functions Definition ---

/// @brief Simulator インスタンスから、そのインスタンスを生成した Factory を取得する関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[out] out_factory_handle 生成された Factory ハンドルを格納するポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorGetFactoryFunc)(SimulatorHandle* sim, FactoryHandle** out_factory_handle, char** out_error);

/// @brief Simulator の現在の状態を Storage に保存する関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[in] storage_handle 保存先の Storage ハンドル
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorSaveFunc)(SimulatorHandle* sim, StorageHandle* storage_handle, char** out_error);

/// @brief Storage から Simulator の状態を読み込む関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[in] storage_handle 読み込み元の Storage ハンドル
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorLoadFunc)(SimulatorHandle* sim, StorageHandle* storage_handle, char** out_error);

/// @brief 指定したフレーム数だけシミュレーションを進める関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[in] frames 進めるフレーム数
/// @param[in] sheet_width シートの幅
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorStepFunc)(SimulatorHandle* sim, const int frames, const float sheet_width, char** out_error);

/// @brief 全てのストーンが停止するまで、または特定の条件を満たすまでシミュレーションを進める関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[in] mode_flag 終了条件フラグ
/// @param[in] sheet_width シートの幅
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorSimulateFunc)(SimulatorHandle* sim, const DigitalCurling_SimulateModeFlag mode_flag, const float sheet_width, char** out_error);

/// @brief ストーンの配置を設定する関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[in] stones 設定するストーンの座標・速度情報へのポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorSetStonesFunc)(SimulatorHandle* sim, const DigitalCurling_StoneCoordinate* stones, char** out_error);

/// @brief 全てのストーンが停止しているか判定する関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[out] out_stopped 停止している場合 true が格納される
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorAreAllStonesStoppedFunc)(SimulatorHandle* sim, bool* out_stopped, char** out_error);

/// @brief 現在のストーンの配置を取得する関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[out] out_stones 取得したストーン情報を格納する構造体へのポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorGetStonesFunc)(SimulatorHandle* sim, DigitalCurling_StoneCoordinate* out_stones, char** out_error);

/// @brief 直前のステップでの衝突情報を取得する関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[out] out_collisions_json 衝突情報を格納したJSON文字列へのポインタ (呼び出し側で解放が必要)
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorGetCollisionsFunc)(SimulatorHandle* sim, char** out_collisions_json, char** out_error);

/// @brief シミュレータの1フレームあたりの時間（秒）を取得する関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[out] out_seconds 取得した秒数を格納するポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorGetSecondsPerFrameFunc)(SimulatorHandle* sim, float* out_seconds, char** out_error);

/// @brief 目標位置に到達するためのショットを計算する関数ポインタ型
/// @param[in] sim Simulator ハンドル
/// @param[in] target_position 目標地点の座標
/// @param[in] target_speed 到達時の目標速度
/// @param[in] angular_velocity 初期の回転角速度
/// @param[out] out_shot 計算された初速・角度を格納するポインタ
/// @param[out] out_error エラー発生時のメッセージを格納するポインタ
/// @return 処理結果のエラーコード
typedef DigitalCurling_ErrorCode (*SimulatorCalculateShotFunc)(SimulatorHandle* sim, const DigitalCurling_Vector2* target_position, const float target_speed, const float angular_velocity, DigitalCurling_Shot* out_shot, char** out_error);

/// @brief シミュレータプラグイン固有のAPI関数テーブル
struct SimulatorApi {
    /// @brief SimulatorインスタンスからFactoryを取得する関数
    SimulatorGetFactoryFunc get_factory;
    /// @brief Simulatorの状態をStorageに保存する関数
    SimulatorSaveFunc save;
    /// @brief StorageからSimulatorの状態を読み込む関数
    SimulatorLoadFunc load;

    /// @brief 指定したフレーム数だけシミュレーションを進める関数
    SimulatorStepFunc step;
    /// @brief 特定の条件を満たすまでシミュレーションを進める関数
    SimulatorSimulateFunc simulate;
    /// @brief ストーンの配置を設定する関数
    SimulatorSetStonesFunc set_stones;

    /// @brief 全てのストーンが停止しているか判定する関数
    SimulatorAreAllStonesStoppedFunc are_all_stones_stopped;
    /// @brief 現在のストーンの配置を取得する関数
    SimulatorGetStonesFunc get_stones;
    /// @brief 直前のステップでの衝突情報を取得する関数
    SimulatorGetCollisionsFunc get_collisions;
    /// @brief シミュレータの1フレームあたりの時間を取得する関数
    SimulatorGetSecondsPerFrameFunc get_seconds_per_frame;

    /// @brief 目標位置に到達するためのショットを計算する関数
    SimulatorCalculateShotFunc calculate_shot;
};


/// @brief プラグインの基本情報
struct PluginInfo {
    /// @brief プラグイン名
    char plugin_name[256];
    /// @brief プラグインの種類 (Player or Simulator)
    DigitalCurling_PluginType plugin_type;
    /// @brief プラグインのバージョン
    unsigned int plugin_version;
};

/// @brief プラグインのAPI関数テーブル
struct PluginApi {
    /// @brief 文字列解放関数
    FreeStringFunc free_string;

    /// @brief Factory生成関数
    CreateFactoryFunc create_factory;
    /// @brief Storage生成関数
    CreateStorageFunc create_storage;
    /// @brief Factory破棄関数
    DestroyObjectFunc destroy_factory;
    /// @brief Storage破棄関数
    DestroyObjectFunc destroy_storage;
    /// @brief Target(Player/Simulator)生成関数
    CreateTargetFunc create_target;
    /// @brief Target(Player/Simulator)破棄関数
    DestroyObjectFunc destroy_target;

    /// @brief Creator(Factor/Storage)状態取得関数
    ObjectCreatorGetStateFunc object_creator_get_state;
    /// @brief Factory状態設定関数
    FactorySetStateFunc factory_set_state;
    /// @brief Storage状態設定関数
    StorageSetStateFunc storage_set_state;

    /// @brief プレイヤー固有API (Simulatorの場合は `nullptr`)
    const PlayerApi* player;
    /// @brief シミュレータ固有API (Playerの場合は `nullptr`)
    const SimulatorApi* simulator;
};


#ifdef DIGITALCURLING_BUNDLE_PLUGINS

/// @brief 静的プラグインを登録する関数
/// @param info プラグインの基本情報
/// @param api プラグインのAPI関数テーブル
void RegisterStaticPlugin(PluginInfo info, PluginApi api);

#endif // DIGITALCURLING_BUNDLE_PLUGINS

/// @}

} // namespace digitalcurling::plugins
