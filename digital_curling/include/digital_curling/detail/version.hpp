/// \file
/// \brief バージョン取得関数を定義します

#ifndef DIGITAL_CURLING_VERSION_HPP
#define DIGITAL_CURLING_VERSION_HPP

#include <cstdint>
#include <string>

namespace digital_curling {

/// \brief バージョンを得る
///
/// \returns バージョン文字列(例: "1.0.0")
std::string GetVersion();



/// \brief メジャーバージョンを得る
///
/// \returns メジャーバージョン
std::uint32_t GetVersionMajor();



/// \brief マイナーバージョンを得る
///
/// \returns マイナーバージョン
std::uint32_t GetVersionMinor();



/// \brief パッチバージョンを得る
///
/// \returns パッチバージョン
std::uint32_t GetVersionPatch();

} // namespace digital_curling

#endif // DIGITAL_CURLING_VERSION_HPP
