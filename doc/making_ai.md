# 思考エンジンの作成方法


思考エンジンは[通信プロトコル](protocol.md)に従って対戦サーバーと通信を行うならばどのような方法で作っても大丈夫です．しかし，TCP/IP通信を行う必要があるなど，技術的なハードルが高いため，思考エンジンのテンプレートコードを用意しています．
テンプレートに従うことでほぼC言語の知識だけで思考エンジンを作成することができますし，
通信プロトコル仕様書を読む必要が無くなります!!

## 環境構築

テンプレートに従って思考エンジンを作成するためには，
下記のツールとライブラリをインストールする必要があります．恐らくここが一番難しいです．
すでに環境構築が終わっている場合はスキップしてください．

- C++コンパイラ(Windowsの場合Visual Studio，Linuxの場合g++等)
- [CMake](https://cmake.org/): C++のビルド用ツール
- [Boost](https://www.boost.org/): C++のライブラリ

### C++コンパイラのインストール

#### Windowsの場合

Visual Studioをインストールしてください．

#### Linuxの場合

新しめのg++をインストールしてください．
Ninja．


### CMakeのインストール


### Boostのインストール



## プロジェクトのセットアップ

思考エンジンのソースコードを書く前に，プロジェクトをセットアップする必要があります．
これは次の流れで行います．

1. gitリポジトリを作成します
2. Digital Curlingライブラリを取り込みます
3. ソースコードを配置します

順を追って説明します．

### 1. gitリポジトリの作成



### 2. Digital Curlingライブラリを取り込む


### 3. ソースコードの配置


```
extern/
CMakeLists.txt
main.cpp
```

ここまででビルドできるはずです．次のコマンドを実行してください．

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DBOX2D_BUILD_TESTBED=OFF ..
cmake --build . --config Debug
```

## 思考エンジンの作成

main.cpp内のTODOコメントがある箇所を編集していきます．

### リリースビルド

リリースビルドを行うことでコンパイラによって最適化が行われ，より高速なバイナリが作成されます．
リリースビルドには次のコマンドを実行します．

```
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBOX2D_BUILD_TESTBED=OFF ..
cmake --build . --config Release
```
