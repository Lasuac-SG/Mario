//

#ifndef MARIO_GAMEMODEL_H
#define MARIO_GAMEMODEL_H

#include "common/EventId.h"
#include "common/EventTrigger.h"

struct Tile {
    float x, y, w, h;
    TileType type;
};

class GameModel {
public:
    GameModel() = default;

    // === 生命周期 ===
    void init();                  // 初始化关卡 + 玩家出生
    void reset();                 // 重置到出生点
    void update(float dt);        // 物理 → 碰撞 → 状态机 → fire 事件

    // === 输入动作（ViewModel 调用）===
    void setMoveLeft(bool p)  { left_ = p; }
    void setMoveRight(bool p) { right_ = p; }
    void setMoveStop()        { left_ = false; right_ = false; }
    void jump();               // 仅 onGround 时生效

    // === 只读查询 ===
    float playerX()  const { return px_; }
    float playerY()  const { return py_; }
    float playerW()  const { return PLAYER_W; }
    float playerH()  const { return PLAYER_H; }
    MarioState playerState() const { return state_; }
    Direction playerFacing() const { return facing_; }
    const std::vector<Tile>& tiles() const { return tiles_; }

    // === 事件 ===
    EventTrigger modelTrigger;

    // === 物理常量 ===
    static constexpr float GRAVITY   = 1500.0f;
    static constexpr float MOVE_SPEED = 250.0f;
    static constexpr float JUMP_VEL  = -520.0f;
    static constexpr float MAX_FALL  = 700.0f;
    static constexpr float PLAYER_W  = 32.0f;
    static constexpr float PLAYER_H  = 48.0f;

private:
    void applyPhysics(float dt);
    void resolveCollisions();
    void updateState();

    float px_ = 100.0f;
    float py_ = 520.0f;
    float vx_ = 0.0f;
    float vy_ = 0.0f;
    bool  left_ = false;
    bool  right_ = false;
    bool  onGround_ = false;
    MarioState state_ = MarioState::IDLE;
    Direction facing_ = Direction::RIGHT;

    std::vector<Tile> tiles_;
};

#endif  // MARIO_GAMEMODEL_H