// 测试辅助工具：stepUntil — 逐帧推进直到条件满足或超时。
// 避免测试中硬编码帧数（对物理常量修改敏感）。

#pragma once

#include "model/PhysicsConfig.h"  // TimeType
#include "model/TileMap.h"

/// 对 Mario / Enemy 等有 step(TimeType, const TileMap&) 接口的实体：
/// 逐帧 step，predicate 为真时立即返回 true；maxFrames 帧后仍未满足返回 false。
template <typename Entity, typename Predicate>
bool stepUntil(Entity& entity, TimeType dt, const TileMap& map, Predicate pred, int maxFrames = 600) {
    for (int i = 0; i < maxFrames; ++i) {
        if (pred()) return true;
        entity.step(dt, map);
    }
    return false;
}

/// GameModel 使用 update(TimeType) 而非 step(TimeType, TileMap)，提供单独重载。
template <typename Predicate>
bool updateUntil(class GameModel& model, TimeType dt, Predicate pred, int maxFrames = 600) {
    for (int i = 0; i < maxFrames; ++i) {
        if (pred()) return true;
        model.update(dt);
    }
    return false;
}
