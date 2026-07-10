//

#ifndef MARIO_TYPE_H
#define MARIO_TYPE_H

#include "common/ICommand.h"
#include "common/EventId.h"

using PositionType = float;
using EventType = uint32_t;
using Notify_Funtion = std::function<void(EventType)>;
using ViewportDim = float;   // 视口宽/高尺寸类型，区分 PositionType（世界坐标）

// 默认逻辑视口宽度/高度（像素），ViewModel 相机 clamp 与 View sf::View 共用
inline constexpr ViewportDim DefaultViewWidth = 800.0f;
inline constexpr ViewportDim DefaultViewHeight = 600.0f;

// 世界坐标原点，相机/实体初始位置
inline constexpr PositionType WorldOrigin = 0.0f;

template <typename T>
class TypeParameter : public ICommandParameter {
public:
  T v_;
  TypeParameter(T v): v_(v) {}
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
using InputActionParameter = TypeParameter<InputAction>;
using TileInfos = std::vector<TileInfo>;

#endif // MARIO_TYPE_H
