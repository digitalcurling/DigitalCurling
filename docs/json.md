# JSONサポート

デジタルカーリングライブラリのほとんどの構造体/クラスは [nlohmann/json](https://github.com/nlohmann/json) を用いたJSON変換をサポートしています。

## 構造体/クラスの変換

一部を除きほぼすべての構造体/クラスは下記の手順でJSONに変換できます。

```cpp
namespace dc = digitalcurling;

dc::GameSetting game_setting;  // 変換元のインスタンス

// to json
nlohmann::json j_out = game_setting;  // JSONオブジェクトに変換
std::string str = j_out.dump();  // 文字列に変換

// from json
dc::GameSetting game_setting_from_json;  // JSONからの復元先
nlohmann::json j_in = nlohmann::json::parse(str);  // 文字列からJSONオブジェクトへ
j_in.get_to(game_setting_from_json);  // JSONオブジェクトから復元
```
