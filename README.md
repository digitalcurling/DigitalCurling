# DigitalCurling

- [README (English Version)](./README-en.md)
- [公式サイト](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)

DigitalCurling はカーリングをシミュレートし、カーリングAI開発を行うためのプラットフォームです。
モジュール化された設計により、シミュレータやプレイヤー（AI）をプラグインとして拡張することが可能です。

## 概要

このリポジトリではカーリングAIおよびシミュレータを開発するためのライブラリとSDKを提供しています。
本システムは `Core`、`Plugin API`、`Plugin Loader`、および標準の `Plugins` (Simulator/Player) から構成されています。

- **カーリングの再現**
  - 実測データに近い挙動をするカーリングシミュレータ (FCV1モデル等)
  - 4人制・2人制ルールのサポート
- **マルチプラットフォーム対応**
  - Windows 10/11
  - macOS
  - Linux (Ubuntu 20.04/22.04 LTS 等)

## ビルド

### 必要要件

#### ツール
- C++17 対応のコンパイラ (GCC, Clang, MSVC, etc.)
- Git
- [CMake](https://cmake.org/) 3.26 以上
- [Doxygen](https://www.doxygen.nl/) (オプション: `DIGITALCURLING_BUILD_DOCS` 有効時のみ)

#### 依存ライブラリ
以下のライブラリは CMake によって自動的に解決されます。
- [nlohmann/json](https://github.com/nlohmann/json)
- [cpp-uuidv7](https://github.com/chromeru0312/cpp-uuidv7)
- [googletest](https://github.com/google/googletest) (オプション: `DIGITALCURLING_BUILD_TEST` 有効時のみ)

### ビルドオプション

CMake実行時に以下のオプションを指定することで、ビルド構成をカスタマイズできます。

| オプション名 | デフォルト値 | 説明 |
| :--- | :--- | :--- |
| `DIGITALCURLING_BUILD_PLAYERS` | `ON` | 標準プレイヤー(Identical, NormalDist)プラグインをビルドします。 |
| `DIGITALCURLING_BUILD_SIMULATORS` | `ON` | 標準シミュレータ(FCV1)プラグインをビルドします。 |
| `DIGITALCURLING_BUILD_PLUGIN_LOADER` | `ON` | プラグイン読み込みライブラリをビルドします。 |
| `DIGITALCURLING_PLUGIN_LOADER_SHARED` | *`OFF`* *1 | `plugin-loader` を共有ライブラリとしてビルドします。`OFF` の場合は静的ライブラリになります。 |
| `DIGITALCURLING_BUNDLE_PLUGINS` | `OFF` | `ON` の場合、プラグインを `plugin-loader` に静的リンク(バンドル)します。動的ロード用のモジュールはビルドされません。 |
| `DIGITALCURLING_PLUGIN_OUTPUT_DIR` | `"plugins"` | ビルドディレクトリからの相対パスで、プラグインモジュールの出力先を指定します。 |
| `DIGITALCURLING_BUILD_TEST` | `OFF` | ユニットテストをビルドします。有効にすると GoogleTest が自動的にダウンロードされます。 |
| `DIGITALCURLING_BUILD_DOCS` | `OFF` | ドキュメント生成ターゲットを追加します（Doxygen等が必要）。 |

> *1: `DIGITALCURLING_PLUGIN_LOADER_SHARED` のデフォルト値は、CMake標準変数 `BUILD_SHARED_LIBS` の設定に従います（通常は `OFF`）。

### 手順

本ライブラリは CMake の `FetchContent` 機能を使用しているため、事前のサブモジュール設定は不要です。

1. このリポジトリをクローンします。
   ```bash
   git clone https://github.com/digitalcurling/DigitalCurling.git
   cd DigitalCurling
   ```

1. プリセットを使用し、ビルドを実行します。
   ```bash
   # Windows の場合 (適切なプリセットを選択)
   cmake --preset windows-x64

   #インストール (必要に応じて)
   cmake --install . --prefix ./install
   ```

## アーキテクチャとディレクトリ構成

- **src/core**: 座標系、ゲームルール、データ構造などの基本定義
- **src/plugin-api**: プラグイン開発および利用に必要なヘッダ群 (SDK)
- **src/plugin-loader**: プラグインのロード・管理を行うライブラリ
- **src/player**: プレイヤープラグインの実装
- **src/simulator**: シミュレータプラグインの実装

## バージョニング

[セマンティックバージョニング 2.0.0](https://semver.org/lang/ja/) に準拠します。

- **メジャーバージョン**: APIの後方互換性を伴わない更新
- **マイナーバージョン**: APIの後方互換性を伴う機能追加
- **パッチバージョン**: バグ修正

> [!WARNING]
> C++の仕様上、コンパイラや標準ライブラリのバージョンが異なるとABI互換性が保たれない場合があります。  
> 動的ロード（DLL/共有ライブラリ）を利用する際は、本体とプラグインを同一の環境でビルドすることを強く推奨します。  
> バージョン不一致によるトラブルを避けたい場合は、`DIGITALCURLING_BUNDLE_PLUGINS=ON` を利用して静的リンクを行ってください。

## ライセンス

[MIT License](./LICENSE)
