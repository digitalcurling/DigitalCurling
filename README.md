# Digital Curling :curling_stone: version 3

- [README (English Version)](./README-en.md)
- [公式サイト](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)

デジタルカーリングはカーリングをシミュレートし，カーリングAI開発を行うためのシステムです．

## 概要

### このリポジトリが提供するもの

- カーリングAIを開発するためのライブラリ
- カーリングAIの対戦を行うためのTCP/IPサーバー

### カーリングシミュレーション

- 実際のカーリングの実測データに近いカーリングシミュレータ
- 現在は通常の4人制に相当するルールのみに対応

### ライブラリ

- 言語: C++17
- ビルドツール: CMake
- マルチプラットフォーム対応
  - Windows，Ubuntu-18.04 LTS(WSL2)にて動作確認済みです．
  - Macでの動作確認は未実施ですが，対応予定です．

## ビルド

1. このリポジトリをクローンします．
1. サブモジュールをセットアップするために， `git submodule update --init --recursive` を実行します．
1. [Boost](https://www.boost.org/)をインストールします．
1. [CMake](https://cmake.org/)をインストールします．
1. CMakeがユーザーの `PATH` に入っていることを確認します．
1. 下記のビルドコマンドを実行します．

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBOX2D_BUILD_TESTBED=OFF ..
cmake --build . --config RelWithDebInfo
```

:warning: CMakeがBoostを見つけられない場合は，環境変数`BOOST_ROOT`にBoostをインストールしたディレクトリを設定してください．

:warning: 現状 static ライブラリのみに対応しています．
また，インストールはサポートしていません．

## AIを作る

TODO

## 試合を行う

1. サーバーをビルドします．[参照](#ビルド)
1. サーバーのコンフィグファイルを作成します. [参照](#サーバーのコンフィグファイル)
1. 下記のコマンドを実行します(実行ファイルが生成される場所や拡張子などはプラットフォームによって若干異なります)．最初のコマンドライン引数はコンフィグファイルへのパスです．下記の例ではコンフィグファイル名は`config.json`です．

```
./digital_curling__server config.json
```

### サーバーのコンフィグファイル

TODO

## GUI

TODO

## バージョニング

セマンティックバージョニングに準拠します．

- メジャーバージョン: ライブラリの互換性を切る更新の際にインクリメントされます．
- マイナーバージョン: ライブラリの互換性を保ったまま機能を追加した際にインクリメントされます．
- パッチバージョン: バグ修正でインクリメントされます．

:warning: メジャーバージョンおよびマイナーバージョン更新ではバイナリ互換性(binary compatibility)は保証されません(開発コスト削減のため)．
パッチバージョンが上がった際のバイナリ互換性は保証します．

## ライセンス

MITライセンス
