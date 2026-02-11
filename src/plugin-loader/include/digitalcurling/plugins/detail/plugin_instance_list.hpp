#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include <variant>
#include <uuidv7/uuidv7.hpp>
#include <uuidv7/generator.hpp>
#include "digitalcurling/plugins/i_plugin_object.hpp"

namespace digitalcurling::plugins::detail {

struct PluginInstanceList {
public:
    template<typename T>
    std::optional<uuidv7::uuidv7> Add(std::shared_ptr<T> instance) {
        std::unique_lock lock(mutex_);
        auto id = uuidv7::uuidv7_generator::generate_default();

        if constexpr (std::is_base_of_v<FactoryHandle, T>) {
            instances_.emplace(id, std::static_pointer_cast<FactoryHandle>(instance));
        } else if constexpr (std::is_base_of_v<StorageHandle, T>) {
            instances_.emplace(id, std::static_pointer_cast<StorageHandle>(instance));
        } else if constexpr (std::is_base_of_v<TargetHandle, T>) {
            instances_.emplace(id, std::static_pointer_cast<TargetHandle>(instance));
        } else {
            static_assert(always_false<T>, "Unsupported type for Add");
        }
        return id;
    }

    template<typename T>
    std::shared_ptr<T> Get(const uuidv7::uuidv7& id) const {
        std::shared_lock lock(mutex_);
        auto it = instances_.find(id);
        if (it == instances_.end()) return nullptr;

        return std::visit([](auto&& ptr) -> std::shared_ptr<T> {
            return std::dynamic_pointer_cast<T>(ptr);
        }, it->second);
    }

    std::vector<uuidv7::uuidv7> GetAllId() const {
        std::shared_lock lock(mutex_);
        std::vector<uuidv7::uuidv7> result;
        for (const auto& [id, info] : instances_) {
            result.push_back(id);
        }
        return result;
    }

    bool Exists(const uuidv7::uuidv7& id) const {
        std::shared_lock lock(mutex_);
        return instances_.find(id) != instances_.end();
    }

    void Clear() {
        std::unique_lock lock(mutex_);
        instances_.clear();
    }
    bool Remove(const uuidv7::uuidv7& id) {
        std::unique_lock lock(mutex_);
        return instances_.erase(id) > 0;
    }

private:
    using InstanceVariant = std::variant<
        std::shared_ptr<FactoryHandle>,
        std::shared_ptr<StorageHandle>,
        std::shared_ptr<TargetHandle>
    >;

    mutable std::shared_mutex mutex_;
    std::unordered_map<uuidv7::uuidv7, InstanceVariant> instances_;
};

} // namespace digitalcurling::plugins::detail
