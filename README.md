# SHT - Shooting Game Engine

A modern C++ game engine specifically designed for bullet-hell shooting games (STG), built with SDL2 and supporting both native and WebAssembly compilation targets.

## Features

- 🎮 **Specialized STG Engine**: Optimized for bullet-hell and shooting game development
- 🚀 **High Performance**: Written in modern C++23 with efficient entity management
- 🌐 **WebAssembly Support**: Seamless compilation to HTML5 via Emscripten
- 🎬 **Advanced Motion Control**: Bezier curve-based movement with `MotionState` (pos/angle/speed) pipeline
- 🔧 **Embedded VM**: Built-in virtual machine for game scripting (`.stg` format)
- 🎨 **Graphics Pipeline**: Hardware-accelerated rendering with SDL2, texture atlas support
- 🔊 **Audio Support**: Full audio mixing and playback capabilities
- ⚙️ **Entity System**: XMacro-driven entity definitions shared between engine and compiler
- 📡 **GCMS**: GameCommandMessageSystem — decoupled inter-system messaging via `std::variant`

## Architecture

### Core Components

```
engine/src/
├── core/           # Core game loop, entity management, and game state
├── graphics/       # Rendering pipeline (SDL2-based, vec2f coordinates)
├── audio/          # Audio and sound effects management
├── VM/             # Virtual machine for STG script execution
├── gcms/           # GameCommandMessageSystem (GCMS)
├── motion/         # MotionState (pos/angle/speed) and Bezier curve pipeline
├── player/         # Player character logic
├── bullet/         # Bullet types (simple, point, player, homing)
├── enemy/          # Enemy types (bezierEnemy, etc.)
├── scenes/         # Scene management (title, play)
├── tables/         # Entity and parameter tables (ParamTable)
└── external/       # Third-party libraries (rapidjson, y9inc)
```

### Key Systems

- **Entity Manager**: Manages spawn, update, and deletion of all game entities. `update()` returns `true` to stay alive, `false` to be removed.
- **Collision System**: Efficient collision detection and response (single-thread, stable up to ~6000 entities at 60 fps in release build)
- **Motion Pipeline**: `MotionState` structure (`vec2f pos`, `float angle`, `float speed`) with Bezier, Line, and Wave decorators
- **Parameter Tables**: Centralized asset and configuration management via `ParamTable`
- **GCMS**: `GameCommand` is a `std::variant` of all command structs. Systems post commands; the game loop dispatches them each tick.
- **Game VM**: Executes compiled `.stg.dat` binary scripts. Supports call stack and loop stack.
- **XMacro Entity System**: `shared/stgEntity.xmac` defines entity memory layouts used by both the engine (for binary parsing in `VM::op_spawn`) and the compiler (`stgc/convert.py`)

### Coordinate Systems

- **Logic / Rendering**: Both use `vec2f` with the **center of the screen as origin**
- **SDL Rendering**: Internally converted to top-left origin integer coordinates at draw time

### Performance (Release Build, Single Thread)

| Entities | Batch Rendering | FPS      |
|----------|----------------|----------|
| 4,000    | Enabled        | 50–60    |
| 10,000   | Enabled        | ~30      |
| 5,000–6,000 | Disabled    | 50–60    |
| 5,000–6,000 | Hybrid      | 60       |
| 7,000    | Hybrid         | ~30      |

> Collision detection is the primary bottleneck. Multi-threading may be considered beyond ~6,000 entities.

## Building

### Requirements

- CMake 3.20 or later
- C++23 compatible compiler (GCC/Clang with `-std=c++23`)
- SDL2, SDL2_image, SDL2_mixer, SDL2_ttf development libraries
- Emscripten SDK (for WebAssembly builds)
- Python 3.11+ (for STG script compiler)

### Build for Desktop

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Build for WebAssembly

```bash
cd engine
mkdir build_wasm && cd build_wasm
emcmake cmake -DCMAKE_TOOLCHAIN_FILE=$EMSDK/cmake/Emscripten.cmake ..
emmake cmake --build .
```

The WebAssembly output will be available as a single `.html` file that can be served directly.

## Usage

```bash
./build/ST
```

The game loads STG scripts from `assets/main.stg.dat` and runs with an 800×800 display window.

## Game Development

### Asset Structure

```
assets/
├── image/          # Sprite atlas (sprites are packed per entity type)
├── audio/          # Sound effects and music
├── eventTable.json # Spawn event parameter definitions
└── entityTable.def # Entity type definitions and properties
script/             # Source STG scripts (.stg)
```

> `assets/image/` is atlas-formatted. Sprites for the same entity type are stored in the same atlas. Entity names must match their image names.

### STG Script Format

Scripts use a simple label-based assembly language. A `:main` label is required.

```
:main
    spawn bezierEnemy x=-200 y=-200 pattern=debug duration=5s
    wait 2
    spawn bezierEnemy x=-100 y=-100 pattern=debug duration=6s
    wait 1
    call :wave2

:wave2
    repeat 3
        spawn bezierEnemy x=-300 y=-300 pattern=debug duration=7s
        wait 1
    end
    ret
```

**VM Instruction Set:**

| Opcode          | Description                            |
|-----------------|----------------------------------------|
| `shutdown`      | Terminate the VM                       |
| `wait N`        | Wait N ticks                           |
| `waitUntil F`   | Wait until flag `F` is set             |
| `spawn TYPE …`  | Spawn an entity with parameters        |
| `call :label`   | Call a subroutine (uses call stack)    |
| `ret`           | Return from subroutine                 |
| `repeat N`      | Loop N times                           |
| `repeatUntil F` | Loop until flag `F` is set             |
| `end`           | End of `repeat`/`repeatUntil` block    |

**Binary format header (`FileHeader`, 18 bytes):**

```
magic (8B) | version (2B) | entry_pc (4B) | code_size (4B)
```

### Compiling STG Scripts

```bash
cd stgc
python stgc.py ../script/test.stg         # compile
python stgc.py ../script/test.stg debug   # compile + dump IR
```

Output is written as `<input>.dat` (e.g. `test.stg.dat`).  
Copy the `.dat` to `assets/` and run `updateSTG.bat` on Windows.

### Adding a New Enemy

1. Create `engine/src/enemy/<name>.h` inheriting `EntityTemplate`
2. Include the header in `engine/src/scenes/playScene.cpp`
3. Define the binary memory layout in `shared/stgEntity.xmac`
4. Add a `cmd::<name>` struct in `engine/src/gcms/gcms.h` and register it in the `GameCommand` variant

### Adding a New Bullet

1. Create `engine/src/bullet/<name>.h` inheriting `EntityTemplate`
2. Include the header in `engine/src/scenes/playScene.cpp`
3. Add a `cmd::<name>` struct in `engine/src/gcms/gcms.h` and register it in the `GameCommand` variant

> Boss entities are a subtype of enemy — follow the enemy procedure above.

## Documentation

API documentation is generated with Doxygen (Japanese):

```bash
doxygen
# Output in docs/html/
```

## Tools

| Tool         | Location        | Description                              |
|--------------|-----------------|------------------------------------------|
| STG Compiler | `stgc/`         | Compiles `.stg` scripts to `.stg.dat`    |
| Position Tool | `tools/pos/`   | Sprite position/animation helper         |
| Atlas Builder | `tools/atlas.py` | Packs sprites into texture atlases     |
| Font Atlas   | `tools/fontAtlas.py` | Generates font texture atlases      |
| STG Updater  | `updateSTG.bat` | Copies compiled STG assets (Windows)     |

## Project Notes

- `spriteID == EntityType` — entity IDs directly correspond to sprite IDs
- `GCMS` (GameCommandMessageSystem) decouples systems: commands are posted each tick and dispatched by the game loop
- `STGEntity::` structs are XMacro-generated from `shared/stgEntity.xmac` and represent the on-disk binary layout
- `cmd::` structs are hand-written, engine-internal representations using signed integers and `vec2f`
- The conversion between them happens in `VM::op_spawn`
- Spawn parameters reference `assets/eventTable.json` for named pattern lookups
- Angles are stored in **radians** internally; the `degree` field in `cmd::simpleBullet` accepts degrees
- Motion patterns are defined in `param.patterns` arrays in `ParamTable`

## License

See [LICENSE](LICENSE) file for details.

## Flow
[Flow(Japanese)](flow.md)

---

Generated by Claude Haiku 4.5

Generated by Claude Sonnet 4.6
