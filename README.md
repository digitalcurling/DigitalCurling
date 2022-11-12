# DigitalCurling3

- [README (English Version)](./README-en.md)
- [マニュアル](https://github.com/digitalcurling/DigitalCurling3/wiki)
- [公式サイト](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)

デジタルカーリングはカーリングをシミュレートし，カーリングAI開発を行うためのシステムです．

## 概要

このリポジトリではカーリングAIを開発するためのライブラリを提供しています．
詳細は[マニュアル](https://github.com/digitalcurling/DigitalCurling3/wiki)を参照ください．

### 仕様

- カーリングの再現
  - 実際のカーリングの実測データに近いカーリングシミュレータ
  - 現在は通常の4人制に相当するルールのみに対応
- 使用言語: C++17
- ビルドツール: CMake
- マルチプラットフォーム対応
  - Windows11，Ubuntu-18.04 LTS(WSL2)にて動作確認済みです．
  - Macでの動作確認は未実施ですが，対応予定です．

## ビルド

1. Gitをインストールします．
1. [CMake](https://cmake.org/)をインストールします．
1. CMakeが `PATH` に入っていることを確認してください．
1. このリポジトリをクローンします．
1. サブモジュールをセットアップするために， `git submodule update --init --recursive` を実行します．
1. 下記のビルドコマンドを実行します．

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

:warning: スタティックライブラリのみに対応しています．

:warning: インストールは現状ではサポートしていません．

## バージョニング

API互換性について[セマンティックバージョニング 2.0.0](https://semver.org/lang/ja/)に準拠します．

- メジャーバージョン: APIの後方互換性を伴わない更新時にインクリメントします．
- マイナーバージョン: APIの後方互換性を伴う機能追加の際にインクリメントします．
- パッチバージョン: APIの後方互換を伴うバグ修正の際にインクリメントします．

APIの後方互換性を伴う更新ではライブラリにリンクするソフトウェア（アプリケーション，ライブラリ）の動作が保証されます．
これはライブラリの更新時にコンパイルエラーが出ないというだけでなく，リンクするソフトウェアの動作に不具合が出たり，ライブラリ関数の実行速度が改悪されたりしないことを意味します．

:warning: ABI互換性については開発コストの都合で考慮しません．従って，このライブラリはスタティックライブラリとしてのみ提供します．

## ライセンス

MITライセンスに準拠します．
