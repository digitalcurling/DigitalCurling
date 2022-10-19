# Digital Curling :curling_stone: 3

- [README (English Version)](./README-en.md)
- [マニュアル(TODO後でリンク貼る)]()
- [公式サイト](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)

デジタルカーリングはカーリングをシミュレートし，カーリングAI開発を行うためのシステムです．

## 概要

このリポジトリではカーリングAIを開発するためのライブラリを提供しています．
詳細は[マニュアル(TODO後でリンク貼る)]()を参照ください．

### 仕様

- カーリングの再現
  - 実際のカーリングの実測データに近いカーリングシミュレータ
  - 現在は通常の4人制に相当するルールのみに対応
- 使用言語: C++17
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
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

:warning: CMakeがBoostを見つけられない場合は，環境変数`BOOST_ROOT`にBoostをインストールしたディレクトリを設定してください．

:warning: 現状 static ライブラリのみに対応しています．
また，インストールはサポートしていません．

## ライセンス

MITライセンスに準拠します．
