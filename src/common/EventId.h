#ifndef MARIO_EVENTID_H
#define MARIO_EVENTID_H

enum class MarioState { IDLE, RUNNING, JUMPING, FALLING, DEAD };
enum class TileType { EMPTY = 0, GROUND = 1, PIPE = 2, PLATFORM = 3, QUESTION, BRICK};
enum class Direction { LEFT, RIGHT };
enum class InputAction { MOVE_LEFT, MOVE_RIGHT, STOP, JUMP, RESTART };

enum class Event : uint32_t {
    // 动作
    MARIO_JUMPED,           // Mario 起跳 (玩家触发且成功离地)

    // 收集
    COIN_COLLECTED,         // 金币被吃到
    MUSHROOM_COLLECTED,     // 蘑菇被吃到
    MARIO_GROWN,            // Mario 变大
    MARIO_SHRUNK,           // Mario 缩小 (Big→Small)

    // 战斗
    ENEMY_STOMPED,          // 踩死敌人
    MARIO_HURT,             // Mario 受伤
    MARIO_DIED,             // 死亡开始 (碰撞致死/超时致死)

    // 场景
    BLOCK_BUMPED,           // 顶到砖块 (问号砖/普通砖)
    BRICK_BROKEN,           // 砖块被 Big Mario 顶碎

    // 关卡
    GOAL_REACHED,           // Mario 碰到终点旗杆
    LEVEL_RESET,            // 关卡状态重置 (死亡重生/主动 reset)
    LEVEL_LOADED,           // 新关卡文件加载完成

    // 兜底
    STATE_CHANGED,          // 每帧常规更新 (物理步进等)
};

#endif  // MARIO_EVENTID_H
