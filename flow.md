# SHT — 弾幕STGエンジン

C++23 製の弾幕シューティングゲームエンジン。  
ネイティブ（SDL2）と WebAssembly（Emscripten）の両方で動作する。

---

## 設計の概要

```
.stg スクリプト
    ↓  stgc (Python コンパイラ)
.stg.dat バイナリ
    ↓  VM (C++ 仮想マシン)
GCMS (GameCommandMessageSystem)
    ↓  std::variant<GameCommand>
各 EntityManager (Enemy / Bullet / Player …)
    ↓  MotionPipeline / Collider
SDL2 レンダラ
```

システム間の直接参照を避けるため、すべての指令は **GCMS** を経由する。  
VM はスクリプトを解釈して `cmd::bezierEnemy` などのコマンドを積み、  
ゲームループが毎フレームそれを取り出して各マネージャに渡す。

---

## ディレクトリ構成

```
SHT/
├── engine/src/
│   ├── core/         ゲームループ・EntityManager・衝突判定
│   ├── VM/           .stg.dat を実行する仮想マシン
│   ├── gcms/         GCMS（GameCommand の variant と dispatch）
│   ├── motion/       MotionState・MotionPipeline・各コントローラ
│   ├── bullet/       弾エンティティ（SimpleBullet / PointBullet …）
│   ├── enemy/        敵エンティティ（BezierEnemy …）
│   ├── player/       自機
│   ├── scenes/       シーン管理（TitleScene / PlayScene）
│   ├── graphics/     SDL2 ラッパ・アトラス描画
│   ├── audio/        SDL2_mixer ラッパ
│   └── tables/       ParamTable（JSON・def ファイルのロード）
├── stgc/             .stg → .stg.dat コンパイラ（Python）
├── shared/           stgEntity.xmac（エンジン・コンパイラ共通定義）
├── script/           .stg ソーススクリプト
├── tools/            アトラス生成・pos ツール
└── memo/             VM命令表・開発メモ
```

---

## エンティティシステム

### XMacro による定義共有

`shared/stgEntity.xmac` にバイナリレイアウトを一元定義する。

```c
ENTITY(bezierEnemy,
    FIELD(s16, x)
    FIELD(s16, y)
    FIELD(s16, pattern, 0)   // optional, default 0
    FIELD(s32, duration, 5000)
)
```

このファイルはエンジン側（`VM::op_spawn` での `.stg.dat` パース）と  
コンパイラ側（`stgc/convert.py` でのバイナリ生成）の両方が参照する。  
エンティティの追加時にずれが起きにくい。

### `cmd::` と `STGEntity::` の分離

| 型 | 使われる場所 | 特徴 |
|----|-------------|------|
| `STGEntity::bezierEnemy` | バイナリ I/O | XMacro 自動生成、`s16`/`s32` |
| `cmd::bezierEnemy` | エンジン内部 | 手書き struct、`int`・`vec2f` |

変換は `VM::op_spawn` で行う。エンジン内部は符号付き整数を基本とする。

### エンティティのライフサイクル

```cpp
// update() が false を返すとその場で削除される
bool update(float dt, GCMS& gcm) {
    if (!alive) return false;
    ms.pos += mp.update(dt, ms);
    return true;
}
```

`EntityManagerBase<T>` が swap-and-pop で O(1) 削除を行い、  
`physWorld` と `entMgr` へのハンドルも同時に解放する。

---

## モーションシステム

各エンティティは `MotionState`（pos / angle / speed）を持つ。

```
MotionSequence          ← フェーズの列（フェーズが終わると次へ）
  └─ MotionPipeline     ← 1フェーズ = Base + Modifier の組み合わせ
       ├─ Base: LineController | BezierController
       └─ Modifier: WaveDecorator | …
```

`MotionPipeline::update()` は速度ベクトル（`vec2f`）を返し、  
呼び出し側が `ms.pos += vel` する。角度は **ラジアン** で管理する。

---

## VM と .stg スクリプト

### 命令セット

| 命令 | 説明 |
|------|------|
| `wait N` | N ティック待機 |
| `waitUntil FLAG` | フラグが立つまで待機 |
| `spawn TYPE …` | エンティティをスポーン |
| `call :LABEL` | サブルーチン呼び出し（callStack） |
| `ret` | サブルーチンから戻る |
| `repeat N` … `end` | N 回ループ（loopStack） |
| `repeatUntil FLAG` … `end` | フラグが立つまでループ |
| `shutdown` | VM を終了 |

### スクリプト例

```
:main
    spawn bezierEnemy x=-200 y=-200 pattern=debug duration=5s
    wait 2
    call :wave

:wave
    repeat 3
        spawn bezierEnemy x=-300 y=-300 pattern=debug duration=7s
        wait 1
    end
    ret
```

- ラベル（`:name`）が関数単位になる。`:main` は必須。
- `spawn` のパラメータは `assets/eventTable.json` のパターン名を参照できる。
- `call` / `repeat` はそれぞれ独立したスタックで管理（最大ネスト 32）。

### バイナリヘッダ（18 バイト）

```
┌──────────┬─────────┬────────────┬────────────┐
│ magic 8B │ ver 2B  │ entry_pc 4B│ code_size 4B│
└──────────┴─────────┴────────────┴────────────┘
magic = 0x6e69424754533979
```

---

## スクリプトのコンパイル

```bash
updateSTG.bat
```

`updateSTG.bat`（Windows）は内部で`stgc.py`を使用する。

---

## ビルド

### 依存関係

- CMake 3.20+
- C++23 コンパイラ（GCC / Clang）
- SDL2, SDL2_image, SDL2_mixer, SDL2_ttf
- Emscripten SDK（WebAssembly ビルド時）
- Python 3.11+（スクリプトコンパイラ）

### ネイティブ

```bash
cd engine
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### WebAssembly

```bash
cd engine
mkdir build_wasm && cd build_wasm
emcmake cmake -DCMAKE_TOOLCHAIN_FILE=$EMSDK/cmake/Emscripten.cmake ..
emmake cmake --build .
# → ST.html が生成される
```

---

## パフォーマンスメモ

シングルスレッド、リリースビルドでの実測値。  
ボトルネックは**衝突判定**（当たり範囲処理）。

| 構成 | エンティティ数 | FPS |
|------|-------------|-----|
| バッチ有効 | 4,000 | 50–60 |
| バッチ有効 | 10,000 | ~30 |
| バッチ無効 | 5,000–6,000 | 50–60 |
| ハイブリッド | 5,000–6,000 | 60 |
| ハイブリッド | 7,000 | ~30 |
| 当たり判定無効 | 数万 | 60 |

当面はシングルスレッドで十分な規模と判断している。

---

## エンティティの追加手順

### 敵（ボスも同様）

1. `engine/src/enemy/<name>.h` に `EntityBase<T>` を継承したクラスを作る
2. `engine/src/scenes/playScene.cpp` に `#include` する
3. `shared/stgEntity.xmac` にバイナリレイアウトを追加する
4. `engine/src/gcms/gcms.h` に `cmd::<name>` struct を追加し、`GameCommand` variant に登録する

### 弾

1. `engine/src/bullet/<name>.h` に `EntityBase<T>` を継承したクラスを作る
2. `engine/src/scenes/playScene.cpp` に `#include` する
3. `engine/src/gcms/gcms.h` に `cmd::<name>` struct を追加し、`GameCommand` variant に登録する

---

## アセット

```
assets/
├── image/          スプライトアトラス（エンティティ名 = 画像名）
├── audio/          SE・BGM
├── eventTable.json spawn パラメータのパターン定義
└── entityTable.def エンティティ型定義
```

同じ種類のスプライトは同じアトラスにまとめる。  
座標系は論理・描画ともに **画面中央を原点とする `vec2f`**。SDL への変換は描画時に行う。

---

## ドキュメント生成

```bash
doxygen
# docs/html/ に出力（日本語）
```

---

## ライセンス

[LICENSE](LICENSE) を参照。