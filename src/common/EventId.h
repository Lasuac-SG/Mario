//

#ifndef MARIO_EVENTID_H
#define MARIO_EVENTID_H

enum class MarioState { IDLE, RUNNING, JUMPING, FALLING };
enum class TileType { EMPTY = 0, GROUND = 1, PIPE = 2, PLATFORM = 3 };
enum class Direction { LEFT, RIGHT };

// Model → ViewModel 通知事件
enum class ModelEvent : uint32_t {
    STATE_CHANGED = 0,  // Model::update() 后触发
};

// ViewModel → View 通知事件
enum class ViewModelEvent : uint32_t {
    RENDER_UPDATE = 0,  // 要求 View 重绘
};

/// View → ViewModel 输入动作（通过 InputActionParameter 传递给 InputCommand）
enum class InputAction { MOVE_LEFT, MOVE_RIGHT, STOP, JUMP, RESTART };

#endif  // MARIO_EVENTID_H