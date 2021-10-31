#ifndef DIGITAL_CURLING_VERSION_HPP
#define DIGITAL_CURLING_VERSION_HPP

#include <cstdint>
#include <string>

namespace digital_curling {



/// <summary>
/// バージョンを得る
/// </summary>
/// <returns>バージョン文字列(例: "1.0.0")</returns>
std::string GetVersion();



/// <summary>
/// メジャーバージョンを得る
/// </summary>
/// <returns>メジャーバージョン</returns>
std::uint32_t GetVersionMajor();



/// <summary>
/// マイナーバージョンを得る
/// </summary>
/// <returns>マイナーバージョン</returns>
std::uint32_t GetVersionMinor();



/// <summary>
/// パッチバージョンを得る
/// </summary>
/// <returns>パッチバージョン</returns>
std::uint32_t GetVersionPatch();



} // namespace digital_curling

#endif // DIGITAL_CURLING_VERSION_HPP
