// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief C-ABI 互換のデータを定義

#pragma once

/// @addtogroup plugin_api
/// @{

/// @brief エラーコードを表す列挙型 (C互換)
typedef enum {
    // --- Success ---

    /// @brief 成功
    DIGITALCURLING_OK = 0,

    // --- Warnings ---

    /// @brief 不明な警告
    DIGITALCURLING_WARN_UNKNOWN = 9,

    // --- Standard Errors ---

    /// @brief プラグインが見つからない
    DIGITALCURLING_ERR_PLUGIN_NOT_FOUND = 10,
    /// @brief プラグインのバージョンが互換性がない
    DIGITALCURLING_ERR_PLUGIN_VERSION_MISMATCH = 11,
    /// @brief プラグインの種類が無効
    DIGITALCURLING_ERR_INVALID_PLUGIN_TYPE = 12,
    /// @brief インスタンスが見つからない
    DIGITALCURLING_ERR_INSTANCE_NOT_FOUND = 13,
    /// @brief 関数が見つからない
    DIGITALCURLING_ERR_FAILED_TO_LOAD_PLUGIN = 19,

    // --- Error Buffers and Memory ---

    /// @brief バッファーがnullptr
    DIGITALCURLING_ERR_BUFFER_NULLPTR = 21,
    /// @brief 引数が無効
    DIGITALCURLING_ERR_INVALID_ARGUMENT = 22,
    /// @brief メモリー割り当てに失敗
    DIGITALCURLING_ERR_MEMORY_ALLOCATION = 23,
    /// @brief キャストに失敗
    DIGITALCURLING_ERR_FAILED_TO_CAST = 24,

    // --- Data Errors ---

    /// @brief データが無効
    DIGITALCURLING_ERR_INVALID_DATA = 30,
    /// @brief JSONのパースエラー
    DIGITALCURLING_ERR_JSON_PARSE = 31,
    /// @brief JSONのデータエラー
    DIGITALCURLING_ERR_JSON_DATA = 32,
    /// @brief UUIDのパースエラー
    DIGITALCURLING_ERR_UUID_PARSE = 33,

    // --- Other Errors ---

    /// @brief その他の例外
    DIGITALCURLING_ERR_OTHER_EXCEPTION = 98,
    /// @brief 不明なエラー
    DIGITALCURLING_ERR_UNKNOWN = 99

} DigitalCurling_ErrorCode;

/// @brief プラグインの種類を表す列挙型 (C互換)
typedef enum {
    /// @brief シミュレータープラグイン
    DIGITALCURLING_PLUGIN_TYPE_SIMULATOR = 0,
    /// @brief プレイヤープラグイン
    DIGITALCURLING_PLUGIN_TYPE_PLAYER = 1,
    /// @brief 不明なプラグインタイプ
    DIGITALCURLING_PLUGIN_TYPE_UNKNOWN = -1
} DigitalCurling_PluginType;

/// @brief 性別を表す列挙型 (C互換)
typedef enum {
    /// @brief 男性
    DIGITALCURLING_GENDER_MALE = 0,
    /// @brief 女性
    DIGITALCURLING_GENDER_FEMALE = 1,
    /// @brief 不明
    DIGITALCURLING_GENDER_UNKNOWN = -1
} DigitalCurling_Gender;

/// @brief シミュレーションの停止条件フラグを表す列挙型 (C互換)
typedef enum {
    /// @brief 全てのストーンが停止するまで
    DIGITALCURLING_SIMULATE_MODE_FULL      = 1 << 0,
    /// @brief いずれかのストーンがシート外に出るまで
    DIGITALCURLING_SIMULATE_MODE_OUT_STONE = 1 << 1,
    /// @brief いずれかのストーンが衝突するまで
    DIGITALCURLING_SIMULATE_MODE_COLLISION = 1 << 2,
} DigitalCurling_SimulateModeFlag;


/// @brief 2Dベクトルを表す構造体 (C互換)
typedef struct {
    /// @brief X座標
    float x;
    /// @brief Y座標
    float y;
} DigitalCurling_Vector2;

/// @brief ストーンの位置と角度を表す構造体 (C互換)
typedef struct {
    /// @brief ストーンの位置
    DigitalCurling_Vector2 position;
    /// @brief ストーンの角度（rad）
    float angle;
} DigitalCurling_Stone;

/// @brief 全てのストーンの状態を表す構造体 (C互換)
typedef struct {
    /// @brief ストーンの状態の配列（最大16個）
    struct {
        /// @brief ストーンの位置
        DigitalCurling_Vector2 position;
        /// @brief ストーンの角度（rad）
        float angle;
        /// @brief ストーンの並進速度
        DigitalCurling_Vector2 translational_velocity;
        /// @brief ストーンの角速度
        float angular_velocity;
    } stones[16];
} DigitalCurling_StoneCoordinate;

/// @brief ショットの情報を表す構造体 (C互換)
typedef struct {
    /// @brief ショットの並進速度
    float translational_velocity;
    /// @brief ショットの角速度
    float angular_velocity;
    /// @brief ショットする方向（rad）
    float release_angle;
} DigitalCurling_Shot;

/// @}
