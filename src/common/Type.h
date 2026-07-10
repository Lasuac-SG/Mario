//

#ifndef MARIO_TYPE_H
#define MARIO_TYPE_H

#include "common/ICommand.h"
#include "common/EventId.h"

using PositionType = float;
using EventType = uint32_t;
using Notify_Funtion = std::function<void(EventType)>;

template <typename T>
class TypeParameter : public ICommandParameter {
public:
  T v_;
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

using PlayerInfo = PlayerInfoStruct;
using TileInfo = TileInfoStruct;

#endif // MARIO_TYPE_H
