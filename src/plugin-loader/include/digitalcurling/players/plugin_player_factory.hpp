// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PluginPlayerFactory を定義

#pragma once

#include <memory>
#include <string>
#include <utility>
#include "digitalcurling/common.hpp"
#include "digitalcurling/players/i_player_factory.hpp"
#include "digitalcurling/players/plugin_player.hpp"

namespace digitalcurling::players {

/// @brief プラグインを経由したプレイヤーのファクトリー
class PluginPlayerFactory : public plugins::WrapperBase<digitalcurling::players::IPlayerFactory> {
public:
    /// @brief コンストラクタ
    /// @param player_id プレイヤーID
    /// @param instance_id インスタンスID
    /// @param owner_resource 実際のインスタンスを持つリソースへの参照
    PluginPlayerFactory(std::string player_id, uuidv7::uuidv7 instance_id, std::weak_ptr<plugins::PlayerPluginResource> owner_resource)
        : WrapperBase(std::move(player_id), std::move(instance_id), std::move(owner_resource))
          { }

    virtual std::unique_ptr<IPlayer> CreatePlayer() const override {
        return ExecuteResourceFunc<std::unique_ptr<IPlayer>>([&](auto resource) {
            auto id = resource->create_target.Execute(GetInstanceId());
            return std::make_unique<PluginPlayer>(GetPluginId(), id, resource);
        });
    }

    virtual std::unique_ptr<IPlayerFactory> Clone() const override {
        auto json = ToJson();
        return ExecuteResourceFunc<std::unique_ptr<IPlayerFactory>>([&](auto resource) {
            auto id = resource->create_factory.Execute(json.dump().c_str());
            return std::make_unique<PluginPlayerFactory>(GetPluginId(), id, resource);
        });
    }

    /// @brief このファクトリーの状態を JSON 形式で取得する
    /// @return JSON オブジェクト
    virtual nlohmann::json ToJson() const override {
        return ExecuteResourceFunc<nlohmann::json>([&](auto resource) {
            auto res = resource->object_creator_get_state.Execute(GetInstanceId());
            return nlohmann::json::parse(res);
        });
    }

    virtual Gender GetGender() const override {
        return ToJson().at("gender").get<Gender>();
    }

    /// @brief 性別を設定する
    /// @param new_gender 新しい性別
    void SetGender(Gender new_gender) {
        auto json = (nlohmann::json {{ "gender", new_gender }}).dump();
        ExecuteResourceFunc<void>([&](auto resource) {
            resource->factory_set_state.Execute(GetInstanceId(), json);
        });
    }
};

} // namespace digitalcurling::players
