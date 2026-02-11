// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief WrapperBase テンプレートクラスを定義

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include "digitalcurling/plugins/plugin_class_type.hpp"
#include "digitalcurling/plugins/plugin_type.hpp"
#include "digitalcurling/plugins/plugin_resource.hpp"

namespace digitalcurling::plugins {

/// @brief プラグインを経由したオブジェクトのラッパーの基底クラス
/// @tparam TBase ラップするオブジェクトの基底クラス
template <typename TBase>
class WrapperBase : public TBase {
public:
    virtual ~WrapperBase() {
        if (auto resource = owner_resource_.lock()) {
            resource->GetInstanceList().Remove(instance_id_);
        }
    }

    /// @cond Doxygen_Suppress
    // Delete copy constructors
    WrapperBase(WrapperBase const&) = delete;
    WrapperBase& operator = (WrapperBase const&) = delete;
    /// @endcond

    /// @brief インスタンスIDを得る
    /// @returns インスタンスID
    uuidv7::uuidv7 GetInstanceId() const { return instance_id_; }

    /// @inheritdoc
    virtual const char* GetId() const noexcept override { return id_.c_str(); }

protected:
    /// @brief このラッパークラスのプラグインタイプ
    static constexpr PluginType Type = PluginTypeFor<TBase>::value;
    /// @brief このラッパークラスを管理するリソースの型
    using TResource = typename PluginTypeTraits<Type>::ResourceType;

    /// @brief コンストラクタ
    /// @param id プラグインID
    /// @param instance_id インスタンスID
    /// @param owner_resource 実際のインスタンスを持つリソースへの参照
    WrapperBase(std::string id, uuidv7::uuidv7 instance_id, std::weak_ptr<TResource> owner_resource)
        : id_(std::move(id)), instance_id_(std::move(instance_id)), owner_resource_(std::move(owner_resource)) {}

    /// @brief プラグインIDを得る
    /// @return プラグインID
    inline std::string const& GetPluginId() const { return id_; }

    /// @brief プラグインリソースを操作する関数を実行するためのユーティリティ
    /// @tparam T 関数の戻り値の型
    /// @param func 実行する関数
    /// @return 関数の戻り値
    template<typename T>
    T ExecuteResourceFunc(std::function<T(std::shared_ptr<TResource>)> func) const {
        std::lock_guard<std::mutex> lock(resource_mutex_);
        return func(GetOwnerResource());
    }

private:
    std::string id_;
    uuidv7::uuidv7 instance_id_;
    std::weak_ptr<TResource> owner_resource_;
    mutable std::mutex resource_mutex_;

    inline std::shared_ptr<TResource> GetOwnerResource() const {
        auto resource = owner_resource_.lock();
        if (!resource)
            throw std::runtime_error("Owner resource has been destroyed for " + ToString(Type) + ": " + id_);
        return resource;
    }
};

} // namespace digitalcurling::plugins
