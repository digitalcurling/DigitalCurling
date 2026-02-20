// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

#pragma once

#include <nlohmann/json.hpp>

namespace digitalcurling::plugins {


/// @brief ターゲットオブジェクトを作成するオブジェクトの基底ハンドル
struct CreatorHandle { virtual ~CreatorHandle() = default; };
/// @brief Factory オブジェクトを表す不透明ハンドル
struct FactoryHandle : public CreatorHandle { virtual ~FactoryHandle() override = default; };
/// @brief Storage オブジェクトを表す不透明ハンドル
struct StorageHandle : public CreatorHandle { virtual ~StorageHandle() override = default; };

/// @brief プラグインオブジェクトの不透明ハンドル
struct TargetHandle { virtual ~TargetHandle() = default; };
/// @brief Player オブジェクトを表す不透明ハンドル
struct PlayerHandle : public TargetHandle { virtual ~PlayerHandle() override = default; };
/// @brief Simulator オブジェクトを表す不透明ハンドル
struct SimulatorHandle : public TargetHandle { virtual ~SimulatorHandle() override = default; };


/// @brief プラグインオブジェクトのインターフェース
class IPluginObject {
public:
    virtual ~IPluginObject() = default;

    /// @brief プラグインオブジェクトのIDを取得する
    /// @return プラグインのID
    virtual const char* GetId() const noexcept = 0;
};

/// @brief プラグインオブジェクトを作成するインターフェース
class IPluginObjectCreator : public IPluginObject, public CreatorHandle {
public:
    virtual ~IPluginObjectCreator() override = default;

    /// @brief プラグインオブジェクトを作成する
    /// @return 生成されたプラグインオブジェクトのハンドル
    virtual TargetHandle* Create() const = 0;

    /// @brief プラグインオブジェクトの情報を JSON 形式で取得する
    /// @return プラグインオブジェクトの情報を含む JSON オブジェクト
    virtual nlohmann::json ToJson() const = 0;
};

} // digitalcurling::plugins
