#ifndef MARIO_TYPE_H
#define MARIO_TYPE_H

#include <string>
#include <vector>

#include "common/EventId.h"
#include "common/ICommand.h"

using PositionType = float;
using EventType = uint32_t; // 将采用Event类型
using Notify_Funtion = std::function<void(EventType)>;
using ViewportDim = float;

inline constexpr ViewportDim DefaultViewWidth = 800.0f;
inline constexpr ViewportDim DefaultViewHeight = 600.0f;

template <typename T>
class TypeParameter : public ICommandParameter {
   public:
    T v_;
    TypeParameter(T v) : v_(v) {}
};

struct PlayerInfoStruct {
    PositionType x, y;
    float width, height;
    MarioState state;
    Direction direction;
};

struct TileInfoStruct {
    PositionType x, y;
    float w, h;
    TileType type;
};

struct EnemyInfoStruct {
    PositionType x, y;
    float w, h;
    Direction direction;
};

struct HudInfoStruct {
    int score;
    int coins;
    int lives;
    int timeRemaining;
    std::string world;
};

using PlayerInfo = PlayerInfoStruct;
using TileInfo = TileInfoStruct;
using EnemyInfo = EnemyInfoStruct;
using HudInfo = HudInfoStruct;
using InputActionParameter = TypeParameter<InputAction>;
using TileInfos = std::vector<TileInfo>;
using EnemyInfos = std::vector<EnemyInfo>;

#endif  // MARIO_TYPE_H
