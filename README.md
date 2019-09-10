# Maze Library

マイクロマウスの迷路探索ライブラリ

## アルゴリズムの概要

### 迷路探索に必要な処理

マイクロマウスで迷路探索を行うためには，主に以下の処理が必要となる．

- 迷路上にある機体の区画位置と進行方向を管理する処理 (Position, Direction)
- 迷路上の全壁の有無と既知未知を管理する処理 (WallIndex, Maze)
- 迷路上のある区画からある区画(の集合)への移動経路を導出する処理 (StepMap)
- スタート区画からゴール区画(の集合)への最短経路を導出する処理 (※)

※最短経路導出は，移動経路導出処理で代用できる．

### 移動経路導出アルゴリズム

探索中に用いる，「ある始点区画」から「ある目的区画集合」への「移動経路」導出処理．

1. 迷路上の全区画のステップマップを用意する．
2. 目的区画に含まれる区画のステップを0とする．
3. そこから壁がない方向の区画へステップを1ずつ増やし，ステップマップを再帰的に更新する．
4. 始点区画からステップが小さくなる方向へ順次進んでいくとやがて目的区画のひとつにたどりつき，それが移動経路となる．

### 最短経路導出アルゴリズム

一番簡単な実装としては，
移動経路導出アルゴリズムの始点をスタート区画に，
目的区画をゴールにすればよい．
ただし，それだけだとターンの多い経路になりがちなので，
直線優先やスラロームのコストを考慮した最短経路が導出できるとよい．
ここでは割愛する．

### 迷路探索アルゴリズム

未知の迷路を探索し，スタートからゴールまでの最短経路を発見するアルゴリズム．

1. ゴール区画までの往路探索走行
   1. 自己位置からゴール区画までの移動経路を，未知壁は壁なしとして導出する．
   2. 上記の経路を未知壁を含む区画に当たるまで進む．
   3. センサによって壁を確認し，迷路情報を更新する．
   4. 1へ戻る．
2. 最短経路を見つける追加探索走行
   1. スタート区画からゴール区画までの最短経路を，未知壁は壁なしとして導出する．
   2. 最短経路上の未知壁を含む区画を目的地とする．
   3. 自己位置から目的地までの移動経路を，未知壁は壁なしとして導出する．
   4. 上記の経路を未知壁を含む区画に当たるまで進む．
   5. センサによって壁を確認し，迷路情報を更新する．
   6. 1へ戻る．
3. スタートに戻る走行
   1. 自己位置からスタート区画までの経路を，未知壁は壁ありとして導出する．
   2. 上記の経路を進んでスタート区画に到達する．

### 最短走行アルゴリズム

1. スタート区画からゴール区画の最短経路を，（念のため）未知壁は壁ありとして導出する．
2. 上記経路を走行する．

## ライブラリ構成

### 名前空間

この迷路探索ライブラリは，すべて `MazeLib` 名前空間に収められている．

### クラス・構造体・共用体・型

| 型         | 意味       | 用途                                                   |
| ---------- | ---------- | ------------------------------------------------------ |
| Position   | 区画位置   | 迷路上の区画の位置を表すクラス．                       |
| Positions  | 位置の配列 | ゴール位置などの位置の集合を表せる．                   |
| Direction  | 方向       | 迷路上の方向（東西南北，左右，斜めなど）を表すクラス． |
| Directions | 方向の配列 | 始点位置を指定することで移動経路を表せる．             |
| WallIndex  | 壁の識別   | 迷路上の壁の位置を連番で表すクラス．壁の管理に使用．   |
| Maze       | 迷路       | 迷路のスタートやゴール位置，壁情報などを保持するクラス |
| StepMap    | 歩数マップ | 足立法の歩数マップを表すクラス．移動経路導出に使用．   |

### 定数

| 定数          | 意味                    | 用途                              |
| ------------- | ----------------------- | --------------------------------- |
| MAZE_SIZE     | 迷路の最大区画          | 16 or 32 などの迷路サイズを定義． |
| MAZE_SIZE_BIT | 迷路の最大区画の bit 数 | bit shift などに使用．            |

## コンピュータでの動作確認

### 依存パッケージ

- gcc
- cmake

### Linux コマンド

```sh
## クローン
git clone https://github.com/kerikun11/MazeLibrary.git
## 移動
cd MazeLibrary
## 作業ディレクトリを作成
mkdir build
cd build
## 初期化(makefileの生成)
cmake ..
## MSYSの場合
cmake .. -G "MSYS Makefiles"
## 実行
make main
```

## 使用例

- [main.cpp](test_study/main.cpp)

## 設定項目

### 迷路サイズ

- [include/Maze.h](include/Maze.h)

```cpp
static constexpr int MAZE_SIZE = 16;
```
