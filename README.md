# MicroMouse Maze Library

マイクロマウスの迷路探索C++ライブラリ

## 目次

- [MicroMouse Maze Library](#micromouse-maze-library)
  - [目次](#%e7%9b%ae%e6%ac%a1)
  - [マイコンでの使用方法](#%e3%83%9e%e3%82%a4%e3%82%b3%e3%83%b3%e3%81%a7%e3%81%ae%e4%bd%bf%e7%94%a8%e6%96%b9%e6%b3%95)
  - [コンピュータでの使用例](#%e3%82%b3%e3%83%b3%e3%83%94%e3%83%a5%e3%83%bc%e3%82%bf%e3%81%a7%e3%81%ae%e4%bd%bf%e7%94%a8%e4%be%8b)
    - [必要なパッケージ](#%e5%bf%85%e8%a6%81%e3%81%aa%e3%83%91%e3%83%83%e3%82%b1%e3%83%bc%e3%82%b8)
    - [コマンド](#%e3%82%b3%e3%83%9e%e3%83%b3%e3%83%89)
  - [アルゴリズムの概要](#%e3%82%a2%e3%83%ab%e3%82%b4%e3%83%aa%e3%82%ba%e3%83%a0%e3%81%ae%e6%a6%82%e8%a6%81)
    - [迷路探索に必要な処理](#%e8%bf%b7%e8%b7%af%e6%8e%a2%e7%b4%a2%e3%81%ab%e5%bf%85%e8%a6%81%e3%81%aa%e5%87%a6%e7%90%86)
    - [探索走行アルゴリズム](#%e6%8e%a2%e7%b4%a2%e8%b5%b0%e8%a1%8c%e3%82%a2%e3%83%ab%e3%82%b4%e3%83%aa%e3%82%ba%e3%83%a0)
    - [移動経路導出アルゴリズム](#%e7%a7%bb%e5%8b%95%e7%b5%8c%e8%b7%af%e5%b0%8e%e5%87%ba%e3%82%a2%e3%83%ab%e3%82%b4%e3%83%aa%e3%82%ba%e3%83%a0)
    - [最短経路導出アルゴリズム](#%e6%9c%80%e7%9f%ad%e7%b5%8c%e8%b7%af%e5%b0%8e%e5%87%ba%e3%82%a2%e3%83%ab%e3%82%b4%e3%83%aa%e3%82%ba%e3%83%a0)
  - [ライブラリ構成](#%e3%83%a9%e3%82%a4%e3%83%96%e3%83%a9%e3%83%aa%e6%a7%8b%e6%88%90)
    - [名前空間](#%e5%90%8d%e5%89%8d%e7%a9%ba%e9%96%93)
    - [クラス・構造体・共用体・型](#%e3%82%af%e3%83%a9%e3%82%b9%e3%83%bb%e6%a7%8b%e9%80%a0%e4%bd%93%e3%83%bb%e5%85%b1%e7%94%a8%e4%bd%93%e3%83%bb%e5%9e%8b)
    - [定数](#%e5%ae%9a%e6%95%b0)
  - [リファレンスの生成](#%e3%83%aa%e3%83%95%e3%82%a1%e3%83%ac%e3%83%b3%e3%82%b9%e3%81%ae%e7%94%9f%e6%88%90)
    - [追加で必要なパッケージ](#%e8%bf%bd%e5%8a%a0%e3%81%a7%e5%bf%85%e8%a6%81%e3%81%aa%e3%83%91%e3%83%83%e3%82%b1%e3%83%bc%e3%82%b8)
    - [生成方法](#%e7%94%9f%e6%88%90%e6%96%b9%e6%b3%95)

--------------------------------------------------------------------------------

## マイコンでの使用方法

マイコンのC++プロジェクトにこのリポジトリを追加して以下の設定を行う．

| 項目              | 値                      |
| ----------------- | ----------------------- |
| include directory | `./include`             |
| source directory  | `./src`                 |
| compile option    | `-std=c++14 -fconcepts` |

--------------------------------------------------------------------------------

## コンピュータでの使用例

ターミナルでの使用例

### 必要なパッケージ

- [git](https://git-scm.com/)
- [gcc, g++](https://gcc.gnu.org/)
- [make](https://www.gnu.org/software/make/)
- [cmake](https://cmake.org/)

### コマンド

サンプルコード [examples/search/main.cpp](examples/search/main.cpp) を実行するコマンドの例

```sh
## 迷路データ(サブモジュール)を含めて複製
git clone --recursive https://github.com/kerikun11/micromouse-maze-library.git
## 移動
cd micromouse-maze-library
## 作業ディレクトリを作成
mkdir build
cd build
## 初期化 (Makefileの生成)
cmake ..
## 実行 (examples/search/main.cpp を実行)
make search
## コマンドラインにアニメーションが流れる
```

--------------------------------------------------------------------------------

## アルゴリズムの概要

以下で説明するアルゴリズムを用いることで，どんな迷路に対しても少なくともひとつの経路を見つけ出すことができる．
ただし，目的地が封鎖されていて経路が存在しない場合はその事実を検出して終了する．

### 迷路探索に必要な処理

マイクロマウスで迷路探索を行うためには，主に以下の処理が必要となる．

- 迷路上にある機体の区画位置と進行方向を識別する処理 (Position, Direction)
- 迷路上の全壁の有無と既知未知を管理する処理 (WallIndex, Maze)
- 迷路上のある区画からある区画(の集合)への移動経路を導出する処理 (StepMap)
- スタート区画からゴール区画(の集合)への最短経路を導出する処理 (※)

※初期段階では最短経路導出処理は移動経路導出処理で代用できる．

### 探索走行アルゴリズム

未知の迷路を探索し，スタートからゴールまでの最短経路を発見するアルゴリズム．

1. ゴール区画までの往路探索走行
   1. 自己位置からゴール区画までの移動経路を，未知壁は壁なしとして導出する．
      1. 経路が存在しない場合は異常終了とする．
   2. 上記の経路を未知壁を含む区画に当たるまで進む．
   3. センサによって壁を確認し，迷路情報を更新する．
   4. 現在位置がゴールでなければ1へ戻る．
2. 最短経路を見つける追加探索走行
   1. スタート区画からゴール区画までの最短経路を，未知壁は壁なしとして導出する．
      1. 経路が存在しない場合は異常終了とする．
   2. 最短経路上の未知壁を含む区画を目的地とする．目的地が空なら終了する．
   3. 自己位置から目的地までの移動経路を，未知壁は壁なしとして導出する．
   4. 上記の経路を未知壁を含む区画に当たるまで進む．
   5. センサによって壁を確認し，迷路情報を更新する．
   6. 1へ戻る．
3. スタートに戻る走行
   1. 自己位置からスタート区画までの経路を，未知壁は壁ありとして導出する．
   2. 上記の経路を進んでスタート区画に到達する．

### 移動経路導出アルゴリズム

探索中に用いる，「ある始点区画」から「ある目的区画集合」への「移動経路」導出処理．

1. 準備
   1. 迷路上の全区画のステップマップを用意する．
2. 展開
   1. 目的区画に含まれる区画のステップを0とする．
   2. そこから壁がない方向の区画へ進むたびにステップを1ずつ増やし，ステップマップを更新する．
   3. ステップの変更がなくなるまでステップマップを再帰的に更新する．
3. 経路導出
   1. 始点区画からステップが小さくなる方向へ順次進んでいくとやがて目的区画のひとつにたどりつき，それが移動経路となる．
   2. 目的区画にたどり着くことなく，ステップが小さくなる方向が存在しなくなった場合，その迷路に解はないので終了する．

### 最短経路導出アルゴリズム

探索中や最短走行前に用いる，「スタート区画」から「ゴール区画の集合」への「最短経路」の導出処理．

一番簡単な実装としては，
移動経路導出アルゴリズムの始点をスタート区画に，
目的区画をゴールにすればよい．
しかしながら，それだけだとターンの多い経路になりがちなので，
直線部分の加速や各ターンのコストを考慮した最短経路が導出できるとよい．

ここでは台形加速を考慮した直線コストのステップマップを紹介する．(斜め走行および各ターンのコストは考慮していない)

1. 準備
   1. 迷路上の全区画のステップマップを用意する．
   2. 台形加速で n マスの移動するときにかかる時間を列挙したコストテーブルを用意する．
   3. コストテーブルの全要素から1マス直線のコストを引き，さらに1マスのスラロームターンにかかる時間を足す．
2. 展開
   1. 目的区画に含まれる区画のステップを0とする．
   2. そこから壁がない方向の区画へ，直線で行けるところまでコストテーブルの値を用いてステップを更新する．
   3. ステップの変更がなくなるまでステップマップを再帰的に更新する．
3. 経路導出
   1. 始点区画からステップが小さくなる方向へ順次進んでいくとやがて目的区画のひとつにたどりつき，それが移動経路となる．
   2. 目的区画にたどり着くことなく，ステップが小さくなる方向が存在しなくなった場合，その迷路に解はないので終了する．

--------------------------------------------------------------------------------

## ライブラリ構成

### 名前空間

この迷路探索ライブラリは，すべて `MazeLib` 名前空間に収められている．

### クラス・構造体・共用体・型

| 型                   | 意味           | 用途                                                       |
| -------------------- | -------------- | ---------------------------------------------------------- |
| MazeLib::Maze        | 迷路           | 迷路のスタート位置やゴール位置，壁情報などを保持するクラス |
| MazeLib::Position    | 区画位置       | 迷路上の区画の位置を表すクラス．                           |
| MazeLib::Positions   | 位置の配列     | ゴール位置などの位置の集合を表せる．                       |
| MazeLib::Direction   | 方向           | 迷路上の方向（東西南北，左右，斜めなど）を表すクラス．     |
| MazeLib::Directions  | 方向の配列     | 始点位置を指定することで移動経路を表せる．                 |
| MazeLib::WallIndex   | 壁の座標       | 迷路上の壁の位置を表すクラス．壁情報の管理に使用．         |
| MazeLib::WallRecord  | 壁の記録       | 区画位置，方向，壁の有無からなるクラス．                   |
| MazeLib::WallRecords | 壁の記録の配列 | 探索の過程の記録などに使用．                               |
| MazeLib::StepMap     | 歩数マップ     | 足立法の歩数マップを表すクラス．移動経路導出に使用．       |

### 定数

| 定数               | 意味               | 用途                                            |
| ------------------ | ------------------ | ----------------------------------------------- |
| MazeLib::MAZE_SIZE | 迷路の一辺の区画数 | 正方形の迷路を仮定．16 or 32 などの定数を定義． |

--------------------------------------------------------------------------------

## リファレンスの生成

コード中のコメントは [Doxygen](http://www.doxygen.jp/) に準拠しているので，APIリファレンスを自動生成することができる．

### 追加で必要なパッケージ

- [doxygen](http://www.doxygen.jp/)
- [graphviz](http://www.graphviz.org/)

### 生成方法

```sh
# CMake により初期化されたディレクトリへ移動する
cd build
# ドキュメントの自動生成
make docs
```

上記コマンドにより `build/docs/html/index.html` にリファレンスが生成される．
