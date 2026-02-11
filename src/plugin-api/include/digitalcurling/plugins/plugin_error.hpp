// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief プラグイン関連のエラーを定義

#pragma once

#include <stdexcept>
#include <string>
#include "digitalcurling/plugins/data_object.h"

namespace digitalcurling::plugins {


/// @brief プラグイン処理におけるエラーを表現する例外クラス
class plugin_error : public std::runtime_error {
public:
    /// @brief コンストラクタ
    /// @param code エラー種別を示すエラーコード
    /// @param message エラーの詳細メッセージ
    explicit plugin_error(DigitalCurling_ErrorCode code, const std::string& message)
        : std::runtime_error(message), error_code_(code) {}

    /// @brief エラーコードを取得する
    /// @return エラーコード
    DigitalCurling_ErrorCode code() const noexcept
    {
        return error_code_;
    }

private:
    DigitalCurling_ErrorCode error_code_;
};

} // namespace digitalcurling::plugins
