//

#ifndef MARIO_ENTITYRENDERER_H
#define MARIO_ENTITYRENDERER_H

#include "common/Type.h"  // PlayerInfo, TileInfo

class AssetManager;

/// 渲染策略基类 —— SimpleRenderer(占位矩形) / SpriteRenderer(将来精灵)
class EntityRenderer {
public:
    virtual ~EntityRenderer() = default;

    virtual void draw(sf::RenderWindow& window,
                      const PlayerInfo& player,
                      AssetManager& assets) = 0;

    virtual void draw(sf::RenderWindow& window,
                      const TileInfo& tile,
                      AssetManager& assets) = 0;
};

#endif  // MARIO_ENTITYRENDERER_H