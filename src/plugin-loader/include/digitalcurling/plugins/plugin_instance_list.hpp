// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief PluginInstanceList を定義

#pragma once

#include <memory>
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include <uuidv7/uuidv7.hpp>
#include <uuidv7/generator.hpp>
#include "digitalcurling/plugins/plugin_class_type.hpp"

namespace digitalcurling::plugins {

/// @brief プラグインインスタンスの管理クラス
struct PluginInstanceList {
public:
    /// @brief インスタンスを追加する
    /// @tparam T インスタンスの型
    /// @param instance 追加するインスタンスへの共有ポインタ
    /// @return インスタンスID
    template<typename T>
    std::optional<uuidv7::uuidv7> Add(std::shared_ptr<T> instance) {
        std::unique_lock lock(mutex_);
        auto id = uuidv7::uuidv7_generator::generate_default();
        instances_.emplace(id, InstanceInfo{GetClassType<T>(), instance});
        return id;
    }

    /// @brief インスタンスを取得する
    /// @tparam T インスタンスの型
    /// @param id インスタンスID
    /// @return インスタンスへの共有ポインタ (存在しない場合は `nullptr`)
    template<typename T>
    std::shared_ptr<T> Get(const uuidv7::uuidv7& id) const {
        std::shared_lock lock(mutex_);
        auto it = instances_.find(id);
        if (it == instances_.end()) return nullptr;

        if constexpr (std::is_same_v<T, CreatorHandle>) {
            if (it->second.class_type == PluginClassType::factory)
                return std::any_cast<std::shared_ptr<FactoryHandle>>(it->second.instance_ptr);
            if (it->second.class_type == PluginClassType::storage)
                return std::any_cast<std::shared_ptr<StorageHandle>>(it->second.instance_ptr);
            return nullptr;
        } else if constexpr (std::is_base_of_v<TargetHandle, T>) {
            if (it->second.class_type != GetClassType<T>()) return nullptr;
            return std::dynamic_pointer_cast<T>(std::any_cast<std::shared_ptr<TargetHandle>>(it->second.instance_ptr));
        } else {
            if (it->second.class_type != GetClassType<T>()) return nullptr;
            return std::any_cast<std::shared_ptr<T>>(it->second.instance_ptr);
        }
    }

    /// @brief すべてのインスタンスIDを取得する
    /// @return インスタンスIDのリスト
    std::vector<uuidv7::uuidv7> GetAllId() const {
        std::shared_lock lock(mutex_);
        std::vector<uuidv7::uuidv7> result;
        for (const auto& [id, info] : instances_) {
            result.push_back(id);
        }
        return result;
    }

    /// @brief インスタンスが存在するか確認する
    /// @param id インスタンスID
    /// @return 該当インスタンスのクラスの種類 (存在しない場合は `std::nullopt`)
    std::optional<PluginClassType> Exists(const uuidv7::uuidv7& id) const {
        std::shared_lock lock(mutex_);
        auto it = instances_.find(id);
        if (it == instances_.end()) return std::nullopt;
        return it->second.class_type;
    }

    /// @brief 全てのインスタンスを削除する
    void Clear() {
        std::unique_lock lock(mutex_);
        instances_.clear();
    }

    /// @brief インスタンスを削除する
    /// @param id インスタンスID
    /// @return 削除に成功したか (存在しない場合は `false`)
    bool Remove(const uuidv7::uuidv7& id) {
        std::unique_lock lock(mutex_);
        return instances_.erase(id) > 0;
    }

private:
    struct InstanceInfo {
        PluginClassType class_type;
        std::any instance_ptr;
    };

    template <typename T>
    PluginClassType GetClassType() const {
        if constexpr (std::is_base_of_v<FactoryHandle, T>) {
            return PluginClassType::factory;
        } else if constexpr (std::is_base_of_v<StorageHandle, T>) {
            return PluginClassType::storage;
        } else if constexpr (std::is_base_of_v<TargetHandle, T>) {
            return PluginClassType::target;
        } else {
            static_assert(!sizeof(T*), "Unsupported type for GetClassType");
        }
    }

    mutable std::shared_mutex mutex_;
    std::unordered_map<uuidv7::uuidv7, PluginInstanceList::InstanceInfo> instances_;
};

} // namespace digitalcurling::plugins
