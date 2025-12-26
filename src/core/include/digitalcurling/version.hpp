// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief Version を定義

#pragma once

#include <charconv>
#include <cstdint>
#include <string>
#include <stdexcept>
#include "digitalcurling/common.hpp"

namespace digitalcurling {


/// @brief バージョン情報を格納する構造体
/// @note バージョン文字列は `[prefix]v[major].[minor].[patch]` 形式
struct Version {
public:
    /// @brief バージョンのプレフィックス
    char prefix[16] = {};
    /// @brief メジャーバージョン
    uint8_t major = 0;
    /// @brief マイナーバージョン
    uint8_t minor = 0;
    /// @brief パッチバージョン
    uint8_t patch = 0;

    /// @brief コンストラクタ
    Version() = default;

    /// @brief コンストラクタ
    /// @param[in] major メジャーバージョン
    /// @param[in] minor マイナーバージョン (省略可)
    /// @param[in] patch パッチバージョン (省略可)
    /// @param[in] prefix プレフィックス (省略可)
    constexpr Version(uint8_t major, uint8_t minor = 0, uint8_t patch = 0, const std::string_view& prefix = "")
        : major(major), minor(minor), patch(patch)
    {
        if (prefix.size() > 0) set_prefix(prefix);
    }

    /// @brief バージョン文字列を解析して `Version` 構造体に変換する
    /// @param[in] str バージョン文字列
    /// @return `Version` 構造体
    /// @throws std::invalid_argument 解析に失敗した場合
    static Version Parse(const std::string_view str) {
        auto output = Version();
        size_t pos = 0, pos2 = 0;

        if ((pos = str.find('v')) == std::string_view::npos)
            throw std::invalid_argument("invalid version string");
        output.set_prefix(str.substr(0, pos));

        uint8_t* ptrs[] = { &output.major, &output.minor, &output.patch };
        for (int i = 0; i < 3; i++) {
            pos2 = pos + 1;
            if ((pos = str.find('.', pos2)) == std::string::npos) pos = str.length();

            auto res = std::from_chars(str.data() + pos2, str.data() + pos, *ptrs[i]);
            if (res.ec != std::errc())
                throw std::invalid_argument("invalid version string");
        }
        return output;
    }
    /// @brief バージョン文字列を解析して `Version` 構造体に変換する
    /// @param[in] str バージョン文字列
    /// @param[out] output 変換結果を格納する `Version` 構造体
    /// @return 変換に成功した場合は `true`
    static bool TryParse(const std::string_view str, Version &output) noexcept {
        output = Version();
        try {
            output = Parse(str);
            return true;
        } catch (...) {
            return false;
        }
    }

    /// @brief バージョン文字列を取得する
    /// @param[in] no_prefix プレフィックスを付けない場合は `true` (Default: `false`)
    /// @return バージョン文字列
    /// @note バージョン文字列は `[prefix]v[major].[minor].[patch]` 形式
    std::string ToString(bool no_prefix = false) const {
        std::string res = 'v' + std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(patch);
        return (no_prefix ? "" : prefix) + res;
    }

    /// @brief バージョンが互換性があるかを判定する
    /// @param[in] rhs 比較対象のバージョン
    /// @return 互換性がある(メジャーバージョンが同じ)場合は `true`
    constexpr bool IsCompatible(Version const& rhs) const {
        return std::string_view(prefix) == std::string_view(rhs.prefix) && major == rhs.major;
    }

    /// @brief バージョンが同じかを判定する
    /// @param[in] rhs 比較対象のバージョン
    /// @return 同じ場合は `true`
    constexpr bool operator==(Version const& rhs) const {
        return IsCompatible(rhs) && minor == rhs.minor && patch == rhs.patch;
    }
    /// @brief バージョンが違うかを判定する
    /// @param[in] rhs 比較対象のバージョン
    /// @return 違う場合は `true`
    constexpr bool operator!=(Version const& rhs) const {
        return !(*this == rhs);
    }

    /// @brief バージョンの大小を比較する
    /// @param[in] rhs 比較対象のバージョン
    /// @return 自分が小さい場合は `true`
    constexpr bool operator<(Version const& rhs) const {
        std::string_view lhs_sv(prefix);
        std::string_view rhs_sv(rhs.prefix);

        if (lhs_sv != rhs_sv) return lhs_sv < rhs_sv;
        if (major != rhs.major) return major < rhs.major;
        if (minor != rhs.minor) return minor < rhs.minor;
        return patch < rhs.patch;
    }
    /// @brief バージョンの大小を比較する
    /// @param[in] rhs 比較対象のバージョン
    /// @return 自分が大きい場合は `true`
    constexpr bool operator>(Version const& rhs) const {
        return rhs < *this;
    }
    /// @brief バージョンの大小を比較する
    /// @param[in] rhs 比較対象のバージョン
    /// @return 自分が小さいか等しい場合は `true`
    constexpr bool operator<=(Version const& rhs) const {
        return !(*this > rhs);
    }
    /// @brief バージョンの大小を比較する
    /// @param[in] rhs 比較対象のバージョン
    /// @return 自分が大きいか等しい場合は `true`
    constexpr bool operator>=(Version const& rhs) const {
        return !(*this < rhs);
    }

private:
    constexpr void set_prefix(const std::string_view& str) {
        if (str.size() >= sizeof(prefix))
            throw std::invalid_argument("prefix too long");

        for (size_t i = 0; i < str.size(); ++i) prefix[i] = str[i];
        prefix[str.size()] = '\0';
    }
};


/// @cond Doxygen_Suppress
//json
inline void to_json(nlohmann::json & j, Version const& v) {
    j = v.ToString();
}
inline void from_json(nlohmann::json const& j, Version & v) {
    std::string str = j.get<std::string>();
    if (!Version::TryParse(str, v))
        throw std::invalid_argument("Invalid version string: " + str);
}
/// @endcond

} // namespace digitalcurling

#include "digitalcurling/detail/version_info.hpp"
