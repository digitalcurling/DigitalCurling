// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief C ABI 互換のプラグインローダー API を定義

#pragma once

#include <cstddef>
#include "digitalcurling_plugin_loader_export.h"
#include "digitalcurling/plugins/loader_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Error Handling ---

/// @brief 最後のエラーメッセージを取得する
/// @param[out] out_buffer エラーメッセージを格納するバッファ
/// @param[in] buffer_size バッファのサイズ
/// @param[out] out_required_size メッセージの格納に必要なサイズ（終端のヌル文字を含む）
/// @return 処理結果を示すエラーコード
/// @note 格納に必要なサイズが必要な場合、`out_buffer` に `nullptr` を渡すことで、必要なサイズを `out_required_size` に取得できます。
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_get_last_error_message(char* out_buffer, size_t buffer_size, size_t* out_required_size);

// --- Snapshot ---

/// @brief スナップショットからデータを取得する
/// @param[in] snapshot スナップショットハンドル
/// @param[out] out_buffer データを格納するバッファ
/// @param[in] buffer_size バッファのサイズ
/// @param[out] out_required_size データの格納に必要なサイズ
/// @return 処理結果を示すエラーコード
/// @note 格納に必要なサイズが必要な場合、`out_buffer` に `nullptr` を渡すことで、必要なサイズを `out_required_size` に取得できます。
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_snapshot_get_data(DigitalCurling_SnapshotHandle snapshot, char* out_buffer, size_t buffer_size, size_t* out_required_size);

/// @brief スナップショットを破棄する
/// @param[in] snapshot 破棄するスナップショットのハンドル
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_destroy_snapshot(DigitalCurling_SnapshotHandle snapshot);

// --- Core Plugin Management ---

/// @brief プラグインをロードする
/// @param[in] plugin_path プラグインファイルのパス
/// @param[out] out_plugin_type ロードされたプラグインの種類
/// @param[out] out_plugin_name ロードされたプラグインの名前
/// @param[in] out_name_buffer_size プラグイン名バッファのサイズ
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_load_plugin(const char* plugin_path, DigitalCurling_PluginType* out_plugin_type, char* out_plugin_name, size_t out_name_buffer_size);

/// @brief ロード済みプラグインのスナップショットを取得する
/// @param[in] plugin_type 取得対象のプラグインタイプ
/// @param[in] separator 区切り文字
/// @param[out] out_snapshot スナップショットハンドル (使用後は `dc_loader_destroy_snapshot` で破棄が必要)
/// @param[out] out_snapshot_size スナップショットのサイズ
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_get_loaded_plugins_snapshot(DigitalCurling_PluginType plugin_type, const char separator, DigitalCurling_SnapshotHandle* out_snapshot, size_t* out_snapshot_size);

/// @brief 指定されたプラグインがロードされているかを確認する
///
/// @param[in] plugin_type 確認対象のプラグインタイプ
/// @param[in] plugin_name 確認対象のプラグイン名
/// @param[out] out_loaded ロードされている場合 `true`
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_is_plugin_loaded(DigitalCurling_PluginType plugin_type, const char* plugin_name, bool* out_loaded);

// --- Creator Common Functions ---

/// @brief クリエイター（Factory または Storage）の状態のスナップショットを取得する
/// @param[in] creator_id クリエイターのUUID
/// @param[out] out_snapshot スナップショットハンドル (使用後は `dc_loader_destroy_snapshot` で破棄が必要)
/// @param[out] out_snapshot_size スナップショットのサイズ
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_creator_get_state_snapshot(const DigitalCurling_Uuid* creator_id, DigitalCurling_SnapshotHandle* out_snapshot, size_t* out_snapshot_size);

// --- Player Instance Management ---

/// @brief プレイヤーファクトリーを作成する
/// @param[in] plugin_name 使用するプラグイン名
/// @param[in] json_config 初期化設定用のJSON文字列
/// @param[out] out_factory_id 作成されたファクトリーのUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_create_player_factory(const char* plugin_name, const char* json_config, DigitalCurling_Uuid* out_factory_id);

/// @brief プレイヤーストレージを作成する
///
/// @param[in] plugin_name 使用するプラグイン名
/// @param[in] json_config 初期化設定用のJSON文字列
/// @param[out] out_storage_id 作成されたストレージのUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_create_player_storage(const char* plugin_name, const char* json_config, DigitalCurling_Uuid* out_storage_id);

/// @brief プレイヤーインスタンスを作成する
/// @param[in] creator_id 作成に使用するクリエイター（Factory または Storage）のUUID
/// @param[out] out_player_id 作成されたプレイヤーのUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_create_player(const DigitalCurling_Uuid* creator_id, DigitalCurling_Uuid* out_player_id);

/// @brief プレイヤーインスタンスを削除する
/// @param[in] instance_id 削除するプレイヤーインスタンスのUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_remove_player_instance(const DigitalCurling_Uuid* instance_id);

// --- Player Actions ---

/// @brief プレイヤーの状態をストレージに保存する
/// @param[in] player_id 保存元のプレイヤーUUID
/// @param[in] storage_id 保存先のストレージUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_player_save(const DigitalCurling_Uuid* player_id, const DigitalCurling_Uuid* storage_id);

/// @brief ストレージからプレイヤーの状態を読み込む
/// @param[in] player_id 読み込み先のプレイヤーUUID
/// @param[in] storage_id 読み込み元のストレージUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_player_load(const DigitalCurling_Uuid* player_id, const DigitalCurling_Uuid* storage_id);

/// @brief プレイヤーの性別を取得する
/// @param[in] player_id プレイヤーUUID
/// @param[out] out_gender 性別
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_player_get_gender(const DigitalCurling_Uuid* player_id, DigitalCurling_Gender* out_gender);

/// @brief プレイヤーのショットを決定する
/// @param[in] player_id プレイヤーUUID
/// @param[in] shot_info 理想的なショット情報
/// @param[out] out_shot 決定されたショット情報
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_player_play(const DigitalCurling_Uuid* player_id, const DigitalCurling_Shot* shot_info, DigitalCurling_Shot* out_shot);

// --- Simulator Instance Management ---

/// @brief シミュレーターファクトリーを作成する
/// @param[in] plugin_name 使用するプラグイン名
/// @param[in] json_config 初期化設定用のJSON文字列
/// @param[out] out_factory_id 作成されたファクトリーのUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_create_simulator_factory(const char* plugin_name, const char* json_config, DigitalCurling_Uuid* out_factory_id);

/// @brief シミュレーターストレージを作成する
/// @param[in] plugin_name 使用するプラグイン名
/// @param[in] json_config 初期化設定用のJSON文字列
/// @param[out] out_storage_id 作成されたストレージのUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_create_simulator_storage(const char* plugin_name, const char* json_config, DigitalCurling_Uuid* out_storage_id);

/// @brief シミュレーターインスタンスを作成する
/// @param[in] creator_id 作成に使用するクリエイター（Factory または Storage）のUUID
/// @param[out] out_simulator_id 作成されたシミュレーターのUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_create_simulator(const DigitalCurling_Uuid* creator_id, DigitalCurling_Uuid* out_simulator_id);

/// @brief シミュレーターインスタンスを削除する
/// @param[in] instance_id 削除するシミュレーターインスタンスのUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_remove_simulator_instance(const DigitalCurling_Uuid* instance_id);

// -- Simulator Actions ---

/// @brief シミュレーターの状態をストレージに保存する
/// @param[in] simulator_id 保存元のシミュレーターUUID
/// @param[in] storage_id 保存先のストレージUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_save(const DigitalCurling_Uuid* simulator_id, const DigitalCurling_Uuid* storage_id);

/// @brief ストレージからシミュレーターの状態を読み込む
/// @param[in] simulator_id 読み込み先のシミュレーターUUID
/// @param[in] storage_id 読み込み元のストレージUUID
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_load(const DigitalCurling_Uuid* simulator_id, const DigitalCurling_Uuid* storage_id);

/// @brief シミュレーター上のストーン配置を設定する
/// @param[in] simulator_id シミュレーターUUID
/// @param[in] stones ストーン座標の配列
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_set_stones(const DigitalCurling_Uuid* simulator_id, const DigitalCurling_StoneCoordinate* stones);

/// @brief シミュレーションを指定フレーム数だけ進める
/// @param[in] simulator_id シミュレーターUUID
/// @param[in] frames 進めるフレーム数
/// @param[in] sheet_width シートの幅
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_step(const DigitalCurling_Uuid* simulator_id, const int frames, const float sheet_width);

/// @brief 指定されたモードでシミュレーションを実行する
/// @param[in] simulator_id シミュレーターUUID
/// @param[in] mode_flag シミュレーションモード
/// @param[in] sheet_width シートの幅
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_simulate(const DigitalCurling_Uuid* simulator_id, const DigitalCurling_SimulateModeFlag mode_flag, const float sheet_width);

/// @brief シミュレーター上の現在のストーン配置を取得する
/// @param[in] simulator_id シミュレーターUUID
/// @param[out] out_stones ストーン座標を格納する配列
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_get_stones(const DigitalCurling_Uuid* simulator_id, DigitalCurling_StoneCoordinate* out_stones);

/// @brief シミュレーターの衝突情報のスナップショットを取得する
/// @param[in] simulator_id シミュレーターUUID
/// @param[out] out_snapshot スナップショットハンドル
/// @param[out] out_snapshot_size スナップショットのサイズ
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_get_collisions_snapshot(const DigitalCurling_Uuid* simulator_id, DigitalCurling_SnapshotHandle* out_snapshot, size_t* out_snapshot_size);

/// @brief すべてのストーンが停止しているかを確認する
/// @param[in] simulator_id シミュレーターUUID
/// @param[out] out_stopped 停止している場合 `true`
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_are_all_stones_stopped(const DigitalCurling_Uuid* simulator_id, bool* out_stopped);

/// @brief 1フレームあたりの秒数を取得する
///
/// @param[in] simulator_id シミュレーターUUID
/// @param[out] out_seconds 1フレームあたりの秒数
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_get_seconds_per_frame(const DigitalCurling_Uuid* simulator_id, float* out_seconds);

/// @brief ターゲットへのショットを計算する
///
/// @param[in] simulator_id シミュレーターUUID
/// @param[in] target_position 目標位置
/// @param[in] target_speed 目標速度
/// @param[in] angular_velocity 角速度
/// @param[out] out_shot 計算結果のショット情報
/// @return 処理結果を示すエラーコード
DigitalCurling_ErrorCode DIGITALCURLING_LOADER_API dc_loader_simulator_calculate_shot(
    const DigitalCurling_Uuid* simulator_id,
    const DigitalCurling_Vector2* target_position,
    const float target_speed,
    const float angular_velocity,
    DigitalCurling_Shot* out_shot
);

#ifdef __cplusplus
}
#endif
