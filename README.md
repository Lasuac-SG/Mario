# 🍄 Mario

A classic Super Mario Bros. clone built with **C++17**, **SFML**, and the **MVVM** architectural pattern. The project features physics-based platforming, enemy AI, and a clean separation of concerns across four layers.

## Architecture Overview

The project follows **strict MVVM** with unidirectional data flow:

```
View ──input + tick──▶ ViewModel ──setMove*/update──▶ Model
View ◀──DTO + event─── ViewModel ◀──getter + event──── Model
```

**Iron rule**: `View → ViewModel → Model → common`. The View never includes model headers or calls model methods directly. The Model knows nothing about the ViewModel or View.

| Layer | Directory | Depends on | Responsibility |
|-------|-----------|------------|----------------|
| `common` | `src/common/` | nothing | Shared enums, type aliases, `EventTrigger` (observer), `ICommand` (command pattern), DTO structs |
| `model` | `src/model/` | `common` | Pure game logic: physics, collision, tile map parsing, enemy AI. No SFML. |
| `viewmodel` | `src/viewmodel/` | `common`, `model` | Mediator: drives model tick, caches model data as DTOs, fires render events, manages camera |
| `view` | `src/view/` | `common`, `viewmodel` | SFML window, input polling, rendering via `GameRenderer` + `AssetManager` |
| `app` | `src/app/` | all layers | Composition root: constructs Model → ViewModel → View and wires them together |

---

## Class Diagrams


### Model Layer

```mermaid
classDiagram
    direction TB

    class Mario {
        +reset(PositionType x, PositionType y) void
        +jump() void
        +step(TimeType dt, const TileMap~map~) void
        +startDeathFall() void
        +bounce(VelocityType speed) void
        +setMoveLeft(bool on) void
        +setMoveRight(bool on) void
        +stop() void
        +x() PositionType
        +y() PositionType
        +vy() VelocityType
        +width() PositionType
        +height() PositionType
        +state() MarioState
        +facing() Direction
        -applyHorizontal(TimeType dt) void
        -applyGravity(TimeType dt) void
        -resolveHorizontal(const TileMap~map~) void
        -resolveVertical(const TileMap~map~) void
        -updateState() void
        -x_ PositionType
        -y_ PositionType
        -vx_ VelocityType
        -vy_ VelocityType
        -onGround_ bool
        -facing_ Direction
        -state_ MarioState
        -deathFalling_ bool
        -moveLeft_ bool
        -moveRight_ bool
    }

    class Enemy {
        +reset(PositionType x, PositionType y, Direction dir) void
        +step(TimeType dt, const TileMap~map~) void
        +kill() void
        +alive() bool
        +x() PositionType
        +y() PositionType
        +width() PositionType
        +height() PositionType
        +facing() Direction
        -reverse() void
        -resolveVertical(const TileMap~map~) void
        -resolveHorizontal(const TileMap~map~) bool
        -atLedge(const TileMap~map~) bool
        -x_ PositionType
        -y_ PositionType
        -vx_ VelocityType
        -vy_ VelocityType
        -onGround_ bool
        -alive_ bool
    }

    class TileMap {
        +TileMap()
        +loadFromString(const string~text~) bool
        +loadFromFile(const string~path~) bool
        +loadFromStream(istream~in~) bool
        +cols() int
        +rows() int
        +at(int col, int row) TileType
        +isSolid(int col, int row) bool
        +isSolidTile(TileType t)$ bool
        +toCol(PositionType x) int
        +toRow(PositionType y) int
        +widthPx() PositionType
        +heightPx() PositionType
        +spawnX() PositionType
        +spawnY() PositionType
        +enemySpawns() const vector~EnemySpawn~
        -tiles_ vector~TileType~
        -enemySpawns_ vector~EnemySpawn~
        -cols_ int
        -rows_ int
        -spawnCol_ int
        -spawnRow_ int
    }


    class GameModel {
        +GameModel()
        +update(TimeType dt) void
        +setMoveLeft(bool on) void
        +setMoveRight(bool on) void
        +setMoveStop() void
        +jump() void
        +reset() void
        +loadLevelFromFile(const string~path~) bool
        +testLoadLevelFromString(const string~text~) bool
        +modelTrigger EventTrigger
        -notifyChanged() void
        -rebuildTiles() void
        -resetLevelState() void
        -spawnEnemies() void
        -resolveEnemyCollisions() bool
        -beginDeath() void
        -respawnAfterDeath() void
        -tileMap_ TileMap
        -mario_ Mario
        -tiles_ vector~Tile~
        -enemies_ vector~Enemy~
        -score_ int
        -coins_ int
        -lives_ int
        -timeRemaining_ float
        -deathInProgress_ bool
        -deathElapsed_ float
    }


    GameModel *-- Mario : mario_
    GameModel *-- TileMap : tileMap_
    GameModel *-- "0..*" Enemy : enemies_
    Mario ..> TileMap : reads
    Enemy ..> TileMap : reads
```

### ViewModel Layer

```mermaid
classDiagram
    direction TB

    class ICommandBase {
        <<interface>>
        +exec(ICommandParameter* p) int
    }

    class InputCommand {
        +InputCommand(GameModel* m)
        +exec(ICommandParameter* p) int
        -model_ GameModel*
    }

    class ViewModel {
        +ViewModel(model, viewW, viewH)
        +~ViewModel()
        +getPlayerInfo() const PlayerInfo*
        +getTileInfos() const TileInfos*
        +getEnemyInfos() const EnemyInfos*
        +getHudInfo() const HudInfo*
        +getCameraX() const PositionType*
        +getCameraY() const PositionType*
        +getActionCommand() ICommandBase*
        +getNextStepCommand() function~void(float)~
        +getResizeCommand() function~void(float,float)~
        +setViewport(w, h) void
        +addNotification(func) void
        -onModelChanged(ev) void
        -syncFromModel() void
        -updateCamera() void
        -tick(dt) void
        -model_ GameModel*
        -vmTrigger EventTrigger
        -funct_callback_Index_ uintptr_t
        -actionCmd_ InputCommand
        -player_info_ PlayerInfo
        -tile_infos_ TileInfos
        -enemy_infos_ EnemyInfos
        -hud_info_ HudInfo
        -cameraX_ PositionType
        -cameraY_ PositionType
        -viewW_ ViewportDim
        -viewH_ ViewportDim
    }

    InputCommand --|> ICommandBase
    ViewModel *-- InputCommand : actionCmd_
    ViewModel *-- EventTrigger : vmTrigger
    ViewModel *-- PlayerInfo : player_info_
    ViewModel *-- "0..*" TileInfo : tile_infos_
    ViewModel *-- "0..*" EnemyInfo : enemy_infos_
    ViewModel *-- HudInfo : hud_info_
    ViewModel o-- GameModel : model_
```

### View Layer

```mermaid
classDiagram
    direction TB

    class GameView {
        +GameView()
        +run() void
        +getRenderNotification() Notify_Funtion
        +setCamera(cx, cy) void
        +setPlayerInfo(p) void
        +setTileInfos(t) void
        +setEnemyInfos(e) void
        +setHudInfo(h) void
        +setNextStepCommand(cmd) void
        +setResizeCommand(cmd) void
        +pushCurrentViewportSize() void
        +getInputHandler() InputHandler*
        -processWindowEvents() void
        -NextStep(dt) void
        -window_ sf::RenderWindow
        -input_ InputHandler
        -renderer_ GameRenderer
        -nextStepCommand_ function~void(float)~
        -resizeCommand_ function~void(float,float)~
        -lastDt_ float
    }

    class InputHandler {
        +setActionCommand(cmd) void
        +pollEvents(window) void
        +dispatchInput() void
        -actionCmd_ ICommandBase*
        -keys_ bool[256]
        -prevJump_ bool
        -prevRestart_ bool
    }

    class GameRenderer {
        +GameRenderer()
        +setCamera(cx, cy) void
        +setPlayerInfo(p) void
        +setTileInfos(t) void
        +setEnemyInfos(e) void
        +setHudInfo(h) void
        +render(window, dt) void
        -drawHud(window) void
        -drawHudBlock(window, label, value, cx) void
        -drawPixelText(window, text, x, y, scale) void
        -measurePixelText(text, scale) float
        -drawPlayer(window, dt) void
        -drawEnemy(window, enemy) void
        -drawTile(window, tile) void
        -drawFallbackTile(window, tile) void
        -assets_ AssetManager
        -cameraX_ const PositionType*
        -cameraY_ const PositionType*
        -playerInfo_ const PlayerInfo*
        -tileInfos_ const TileInfos*
        -enemyInfos_ const EnemyInfos*
        -hudInfo_ const HudInfo*
        -runAnimationTime_ float
        -enemyAnimationTime_ float
        -lastPlayerState_ MarioState
        -rect_ sf::RectangleShape
    }

    class AssetManager {
        +load(path) const sf::Texture&
        -cache_ unordered_map~string, Texture~
        -fallback_ sf::Texture
    }

    GameView *-- InputHandler : input_
    GameView *-- GameRenderer : renderer_
    GameRenderer *-- AssetManager : assets_
    InputHandler o-- ICommandBase : actionCmd_
    GameRenderer ..> PlayerInfo : reads
    GameRenderer ..> TileInfo : reads
    GameRenderer ..> EnemyInfo : reads
    GameRenderer ..> HudInfo : reads
```

### App Layer (Composition Root)

```mermaid
classDiagram
    direction TB

    class App {
        +App()
        +initialize() void
        +run() void
        -model_ unique_ptr~GameModel~
        -vm_ unique_ptr~ViewModel~
        -view_ unique_ptr~GameView~
    }

    class GameModel {
        +update(dt)
        +setMoveLeft(on)
        +setMoveRight(on)
        +jump()
        +modelTrigger EventTrigger
    }

    class ViewModel {
        +getPlayerInfo()
        +getTileInfos()
        +getCameraX()
        +getActionCommand()
        +getNextStepCommand()
        +getResizeCommand()
        +addNotification()
    }

    class GameView {
        +run()
        +getRenderNotification()
        +setPlayerInfo()
        +setCamera()
        +getInputHandler()
        +setNextStepCommand()
        +setResizeCommand()
    }

    App *-- GameModel : model_
    App *-- ViewModel : vm_
    App *-- GameView : view_
```

---

## Data Flow

### Input (Downstream)

```
Keyboard → InputHandler::pollEvents → dispatchInput
  → ViewModel::act_Command(InputActionParameter)
    → InputCommand::exec
      → GameModel::setMoveLeft / setMoveRight / jump / reset
```

### Tick & Notification (Upstream)

```
GameView::run (fixed 60 Hz timestep)
  → ViewModel::tick(dt)
    → GameModel::update(dt)
      → Mario + Enemy physics
        → modelTrigger.fire(STATE_CHANGED)
          → ViewModel::onModelChanged
            → syncFromModel (model → DTOs)
              → vmTrigger.fire(RENDER_UPDATE)
                → GameView re-renders
```

### Render Pipeline

```
GameRenderer::render
  1. Draw tiles (GROUND brick texture, PIPE procedural, PLATFORM spritesheet)
  2. Draw enemies (2-frame animation, Direction-aware)
  3. Draw player (state-based animation: idle/run/jump/fall/dead)
  4. Draw HUD (pixel font, 7×5 dot matrix with shadow)
```

---

## Key Design Patterns

| Pattern | Implementation | Purpose |
|---------|---------------|---------|
| **Observer** | `EventTrigger` — generic callbacks via `Notify_Funtion` handles | Model → ViewModel → View event propagation |
| **Command** | `ICommandBase` / `ICommandParameter` / `TypeParameter<T>` / `InputCommand` | Decouple input from action execution |
| **Facade** | `GameModel` | Single entry point for ViewModel, hiding Mario + Enemy + TileMap internals |
| **DTO** | `PlayerInfo`, `TileInfo`, `EnemyInfo`, `HudInfo` | Cross-layer data transfer without exposing model internals |
| **Dependency Injection** | `App::initialize()` | Composition root wires all layers together |

---

## Project Structure

```
Mario/
├── src/
│   ├── common/           # Shared types, enums, EventTrigger, ICommand, DTOs
│   │   ├── EventId.h
│   │   ├── EventTrigger.h
│   │   ├── ICommand.h
│   │   └── Type.h
│   ├── model/            # Pure game logic (no SFML)
│   │   ├── Enemy.h       # header-only
│   │   ├── GameModel.h / .cpp
│   │   ├── Mario.h / .cpp
│   │   ├── PhysicsConfig.h
│   │   ├── Tile.h
│   │   └── TileMap.h / .cpp
│   ├── viewmodel/        # Mediator: model ↔ view
│   │   ├── ViewModel.h / .cpp
│   │   └── command/
│   │       └── Commands.h / .cpp
│   ├── view/             # SFML rendering & input
│   │   ├── View.h / .cpp
│   │   ├── input/
│   │   │   └── InputHandler.h / .cpp
│   │   └── renderer/
│   │       ├── AssetManager.h
│   │       └── GameRenderer.h / .cpp
│   └── app/              # Composition root
│       └── App.h / .cpp
├── tests/                # GTest unit tests
│   ├── common/
│   └── model/
├── picture/              # Game assets (textures, levels)
├── CMakeLists.txt
└── vcpkg.json
```

---

## Build & Run

### Prerequisites

- CMake 3.20+
- Ninja
- vcpkg (manifest mode — SFML and GTest are auto-fetched)

### Configure & Build

```powershell
# Configure (CMake + vcpkg manifest mode)
cmake -DCMAKE_MAKE_PROGRAM=ninja -G "Ninja Multi-Config" `
  -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake `
  -S . -B build

# Debug build
cmake --build build --config Debug

# Release build
cmake --build build --config Release

# Run
.\build\Debug\mario.exe
```

### Tests

```powershell
# Build test targets
cmake --build build --config Debug --target test_common test_model

# Run all tests
ctest --test-dir build -C Debug

# Run with filter
.\build\Debug\test_model.exe --gtest_filter=MarioTest.*
```

---

## Controls

| Key | Action |
|-----|--------|
| ← → / A D | Move left / right |
| Space / W / ↑ | Jump (edge-triggered, ground only) |
| R | Restart level |

## Game Rules

- **Lives**: Start with 3. Touching an enemy or running out of time = death (1.2s bounce animation → lose 1 life → level reset).
- **Time**: 300-second countdown, reaching zero kills Mario.
- **Stomp**: Landing on an enemy from above = stomp (+100 points + bounce). Side contact = Mario dies.
- **Respawn**: After death, the level resets (score/coins/enemies/time/position), only the lost life persists.

---

## License

This is an educational project for ZJU Learning.
