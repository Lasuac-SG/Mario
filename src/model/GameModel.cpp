//

#include "model/GameModel.h"


void GameModel::init() {
    px_ = 100.0f;
    py_ = 520.0f;
    vx_ = 0.0f;
    vy_ = 0.0f;
    left_ = false;
    right_ = false;
    onGround_ = false;
    state_ = MarioState::IDLE;
    facing_ = Direction::RIGHT;

    tiles_.clear();

    // 地面：一整行
    tiles_.push_back({0.0f, 568.0f, 800.0f, 32.0f, TileType::GROUND});

    // 浮空平台（3 个）
    tiles_.push_back({150.0f, 440.0f, 120.0f, 16.0f, TileType::PLATFORM});
    tiles_.push_back({350.0f, 350.0f, 120.0f, 16.0f, TileType::PLATFORM});
    tiles_.push_back({550.0f, 440.0f, 120.0f, 16.0f, TileType::PLATFORM});

    // 管道装饰
    tiles_.push_back({680.0f, 520.0f, 40.0f, 48.0f, TileType::PIPE});
}

void GameModel::reset() {
    px_ = 100.0f;
    py_ = 520.0f;
    vx_ = 0.0f;
    vy_ = 0.0f;
    left_ = false;
    right_ = false;
    onGround_ = false;
    state_ = MarioState::IDLE;
    facing_ = Direction::RIGHT;

    modelTrigger.fire(static_cast<uint32_t>(ModelEvent::STATE_CHANGED));
}

void GameModel::jump() {
    if (onGround_) {
        vy_ = JUMP_VEL;
        onGround_ = false;
        state_ = MarioState::JUMPING;
    }
}

void GameModel::update(float dt) {
    applyPhysics(dt);
    resolveCollisions();
    updateState();
    modelTrigger.fire(static_cast<uint32_t>(ModelEvent::STATE_CHANGED));
}

void GameModel::applyPhysics(float dt) {
    // 读输入标志，设 vx
    if (left_ && !right_) {
        vx_ = -MOVE_SPEED;
        facing_ = Direction::LEFT;
    } else if (right_ && !left_) {
        vx_ = MOVE_SPEED;
        facing_ = Direction::RIGHT;
    } else {
        vx_ = 0.0f;
    }

    // 重力
    vy_ += GRAVITY * dt;
    if (vy_ > MAX_FALL) vy_ = MAX_FALL;

    // 位置更新
    px_ += vx_ * dt;
    py_ += vy_ * dt;
}

void GameModel::resolveCollisions() {
    const float playerLeft   = px_;
    const float playerRight  = px_ + PLAYER_W;
    const float playerTop    = py_;
    const float playerBottom = py_ + PLAYER_H;

    for (const auto& tile : tiles_) {
        const float tLeft   = tile.x;
        const float tRight  = tile.x + tile.w;
        const float tTop    = tile.y;
        const float tBottom = tile.y + tile.h;

        // AABB 粗测
        if (playerRight <= tLeft || playerLeft >= tRight ||
            playerBottom <= tTop || playerTop >= tBottom)
            continue;

        // 计算各方向穿透距离
        const float overlapLeft   = playerRight - tLeft;
        const float overlapRight  = tRight - playerLeft;
        const float overlapTop    = playerBottom - tTop;
        const float overlapBottom = tBottom - playerTop;

        // 最小穿透方向
        const float minOverlapX = std::min(overlapLeft, overlapRight);
        const float minOverlapY = std::min(overlapTop, overlapBottom);

        if (minOverlapX < minOverlapY) {
            // X 方向解决
            if (overlapLeft < overlapRight) {
                px_ = tLeft - PLAYER_W;   // 推向左
            } else {
                px_ = tRight;              // 推向右
            }
        } else {
            // Y 方向解决
            if (overlapTop < overlapBottom) {
                // 从上方落下（踩踏）
                py_ = tTop - PLAYER_H;
                vy_ = 0.0f;
                onGround_ = true;
            } else {
                // 从下方顶到头
                py_ = tBottom;
                vy_ = 0.0f;
            }
        }
    }

    // 边界限制
    if (px_ < 0.0f) px_ = 0.0f;
    if (px_ > 800.0f - PLAYER_W) px_ = 800.0f - PLAYER_W;

    // 掉出屏幕底部 → 复位
    if (py_ > 600.0f) {
        px_ = 100.0f;
        py_ = 520.0f;
        vx_ = 0.0f;
        vy_ = 0.0f;
        onGround_ = false;
    }
}

void GameModel::updateState() {
    if (onGround_) {
        state_ = (std::fabs(vx_) > 1.0f) ? MarioState::RUNNING : MarioState::IDLE;
    } else {
        state_ = (vy_ < 0.0f) ? MarioState::JUMPING : MarioState::FALLING;
    }
}