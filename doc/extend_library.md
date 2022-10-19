# ライブラリの拡張

ここでは新しい物理シミュレータや新しいプレイヤーを追加する方法を説明します．

## 物理シミュレータの追加

1. [ISimulator](@ref digitalcurling3::ISimulator)，
   [ISimulatorFactory](@ref digitalcurling3::ISimulatorFactory)，
   [ISimulatorStorage](@ref digitalcurling3::ISimulatorStorage)を継承したクラスを実装します．
2. 実装した[ISimulatorFactory](@ref digitalcurling3::ISimulatorFactory)と
   [ISimulatorStorage](@ref digitalcurling3::ISimulatorStorage)のJSON変換関数を実装します．
   - それぞれクラスと同じ名前空間内に `to_json` ， `from_json` 関数を定義します．
     これらの関数の詳細については [nlohmann/json](https://github.com/nlohmann/json) を参照してください．
     \emoji warning 両クラスとも変換するJSONのタイプはオブジェクトとし，必ず `"type"` に **シミュレータ識別名** を設定してください．
     シミュレータ識別名は `GetSimulatorId` 関数の返り値と一致させるのが望ましいです．
3. インターフェースを介したJSON変換の登録を行います．
   2つの方法があります．
   - ライブラリ使用者向けの方法: この方法では，ライブラリをリンクしている側で実行時にJSON変換を新規登録できます．
     1. [polymorphic_json::Register()](@ref digitalcurling3::polymorphic_json::Register) を使用し， `polymorphic_json::Register<ISimulatorFactory, 作成したファクトリクラス>(シミュレータ識別名);` 及び `polymorphic_json::Register<ISimulatorStorage, 作成したストレージクラス>(シミュレータ識別名);` のような呼出しを行うことで，インターフェースを介したJSON変換の登録を行えます．[polymorphic_json::Register()](@ref digitalcurling3::polymorphic_json::Register)の呼出しはJSONの変換を行う前に各クラスに対し1回だけ行います．
   - ライブラリ開発者向けの方法: ライブラリのソースコード自体を変更する方法です．
     1. `src/i_simulator_factory.cpp`，`src/i_simulator_storage.cpp`の該当箇所を編集します．
        編集方法は該当ファイルを参照してください．


## プレイヤーの追加

シミュレータの追加方法と根本的に違いはありません．
物理シミュレータの追加方法の `ISimulator` を `IPlayer` に， `ISimulatorFactory` を `IPlayerFactory` に， `ISimulatorStorage` を `IPlayerStorage` に読み替えてください．
