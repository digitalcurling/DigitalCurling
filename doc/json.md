# JSONサポート

digital_curlingライブラリのほとんどの構造体/クラスついては[nlohmann/json](https://github.com/nlohmann/json)を用いたJSON変換をサポートしています．

## 構造体/クラスの変換

一部を除きほぼすべての構造体/クラスは下記の手順でJSONに変換できます．

```cpp
namespace dc = digital_curling;

dc::Vector2 vector2(1.0f, 2.0f);
dc::GameSetting game_setting;

// to json
nlohmann::json j_vector = vector2;
nlohmann::json j_game_setting = game_setting;

// from json
dc::Vector2 vector2_from_json;
dc::GameSetting game_setting_from_json;

j_vector.get_to(vector2_from_json);
j_game_setting.get_to(game_setting_from_json);
```

`nlohmann::json`型のオブジェクトに変換した後の文字列への変換等については
[nlohmann/json](https://github.com/nlohmann/json)本家のリファレンスを参照ください．

## インターフェースを介したクラスの変換

いくつかのインターフェースは下記の手順でJSONに変換することができます．

```cpp
namespace dc = digital_curling;

std::unique_ptr<dc::ISimulatorFactory> simulator_factory = std::make_unique<dc::FCV1SimulatorFactory>();

// to json
nlohmann::json j_simulator_factory = simulator_factory;

// from json
std::unique_ptr<dc::ISimulatorFactory> simulator_factory_from_json = j_simulator_factory.get<std::unique_ptr<dc::ISimulatorFactory>>();
// j_simulator_factory.get_to(simulator_factory_from_json); は使えません
```

### 上記方法で変換可能なインターフェースの一覧

- [ISimulatorFactory](@ref digital_curling::ISimulatorFactory)
- [ISimulatorStorage](@ref digital_curling::ISimulatorStorage)
- [IPlayerFactory](@ref digital_curling::IPlayerFactory)
- [IPlayerStorage](@ref digital_curling::IPlayerStorage)
