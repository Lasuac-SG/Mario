#ifndef MARIO_EVENTID_H
#define MARIO_EVENTID_H

enum class MarioState { IDLE, RUNNING, JUMPING, FALLING, DEAD };
enum class TileType { EMPTY = 0, GROUND = 1, PIPE = 2, PLATFORM = 3 };
enum class Direction { LEFT, RIGHT };
enum class ModelEvent : uint32_t { STATE_CHANGED = 0, };
enum class ViewModelEvent : uint32_t { RENDER_UPDATE = 0, };
enum class InputAction { MOVE_LEFT, MOVE_RIGHT, STOP, JUMP, RESTART };

#endif  // MARIO_EVENTID_H
