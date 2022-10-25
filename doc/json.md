# JSONサポート

デジタルカーリングライブラリのほとんどの構造体/クラスついては[nlohmann/json](https://github.com/nlohmann/json)を用いたJSON変換をサポートしています．

## 構造体/クラスの変換

一部を除きほぼすべての構造体/クラスは下記の手順でJSONに変換できます．

```cpp
namespace dc = digitalcurling3;

dc::GameSetting game_setting;  // 変換元のインスタンス

// to json
nlohmann::json j_out = game_setting;  // JSONオブジェクトに変換
std::string str = j_out.dump();  // 文字列に変換

// from json
dc::GameSetting game_setting_from_json;  // JSONからの復元先
nlohmann::json j_in = nlohmann::json::parse(str);  // 文字列からJSONオブジェクトへ
j_in.get_to(game_setting_from_json);  // JSONオブジェクトから復元
```

## インターフェースの変換

いくつかのインターフェースは下記の手順でJSONに変換することができます．

```cpp
namespace dc = digitalcurling3;

// 変換元のインスタンス
std::unique_ptr<dc::ISimulatorFactory> simulator_factory = std::make_unique<dc::SimulatorFCV1Factory>();

// to json
nlohmann::json j_out = simulator_factory; // JSONオブジェクトに変換
std::string str = j_out.dump();  // 文字列に変換

// from json
nlohmann::json j_in = nlohmann::json::parse(str);  // 文字列からJSONオブジェクトへ
std::unique_ptr<dc::ISimulatorFactory> simulator_factory_from_json = j_in.get<std::unique_ptr<dc::ISimulatorFactory>>(); // JSONオブジェクトから復元
// 構造体/クラスの変換で使用していた get_to はインターフェースの変換の場合は使えません
```

### 上記方法で変換可能なインターフェースの一覧

- [ISimulatorFactory](@ref digitalcurling3::ISimulatorFactory)
- [ISimulatorStorage](@ref digitalcurling3::ISimulatorStorage)
- [IPlayerFactory](@ref digitalcurling3::IPlayerFactory)
- [IPlayerStorage](@ref digitalcurling3::IPlayerStorage)

### ユーザー実装のインターフェースの変換

ユーザー実装のインターフェースも上記方法で変換可能です．
[ライブラリの拡張](extend_library.md)の方法に従ってください．